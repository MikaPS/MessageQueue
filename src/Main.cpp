
#include "EasyBMP.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <memory>
#include <future>

#include <string>

#include <thread>

#include <unistd.h>
#include <mutex>
#include <chrono>

#include "Fractal.h"
#include "FractalMessage.h"
#include "ThreadSafeQueue.h"

using namespace std;

typedef TSQueue<shared_ptr<FractalMessage>> FractalMessageQueue;
typedef vector<shared_ptr<thread>> ThreadVector;

// CONSUMER FUNCTION, uses the messages using queue
void render_fractal(FractalMessageQueue *queue)
{
    while (true)
    {
        std::shared_ptr<FractalMessage> msg = queue->pop();
        if (msg == nullptr)
        {
            return;
        }
        // actually render a tile of the fractal by feeding Fractal.h::fractal with the data
        // that outputs to the output image
        fractal(msg->get_output_image(), msg->get_left(), msg->get_top(), msg->get_x_size(), msg->get_y_size(),
                msg->get_min_x(), msg->get_min_y(), msg->get_max_x(), msg->get_max_y(),
                msg->get_image_x(), msg->get_image_y());
    }
};

// PRODUCER FUNCTION, creates messages
void do_tile(FractalMessageQueue *queue, EasyBMP::Image *img, float left, float top, float xsize, float ysize, float img_min_x, float img_min_y, float img_max_x, float img_max_y, float img_x_size, float img_y_size)
{
    // "Produce" a number of messages into the queue
    for (size_t i = 0; i < 100; i++)
    {
        std::shared_ptr<FractalMessage> msg = std::make_shared<FractalMessage>(img, left, top, xsize, ysize, img_min_x, img_min_y, img_max_x, img_max_y, img_x_size, img_y_size);
        queue->push(msg);
    }
};


int main()
{
    // Define an RGB color for black
    EasyBMP::RGBColor black(0, 0, 0);

    // make an image initialized with black .  Image size is 512x512
    // Image size should always be POT (power of 2)
    size_t img_size_x = 512;
    size_t img_size_y = 512;

    EasyBMP::Image img(img_size_x, img_size_y, "output.bmp", black);

    // the number of tiles in X & Y
    size_t num_tiles_x = 4;
    size_t num_tiles_y = 4;

    // this is the size of the tiles in the output image
    size_t img_tile_size_x = img_size_x / num_tiles_x;
    size_t img_tile_size_y = img_size_y / num_tiles_y;

    // the overall area we are rendering in "fractal space"
    float left = -1.75;
    float top = -0.25;
    float xsize = 0.25;
    float ysize = 0.45;

    // how big each tile is in x,y in "fractal space"
    float fractal_tile_size_x = xsize / num_tiles_x;
    float fractal_tile_size_y = ysize / num_tiles_y;

    FractalMessageQueue queue;
    ThreadVector threads;

    for (size_t i = 0; i < num_tiles_x; i++)
    {
        for (size_t j = 0; j < num_tiles_y; j++)
        {
            // calculate the coords for the output tile, in bitmap(img) space
            size_t img_tile_min_x = 0 + (img_tile_size_x * i);
            size_t img_tile_min_y = 0 + (img_tile_size_y * j);

            size_t img_tile_max_x = img_tile_min_x + img_tile_size_x;
            size_t img_tile_max_y = img_tile_min_y + img_tile_size_y;

            // calculate the coords for the output tile in fractal space
            float fractal_tile_min_x = left + (i * fractal_tile_size_x);
            float fractal_tile_min_y = top + (j * fractal_tile_size_y);
            float fractal_tile_max_x = fractal_tile_min_x + fractal_tile_size_x;
            float fractal_tile_max_y = fractal_tile_min_y + fractal_tile_size_y;

            cout << "TILE: " << i << "," << j <<  " img_tile_min_x : " << img_tile_min_x << " img_tile_min_y : " << img_tile_min_y  \
                << " img_tile_max_x : " << img_tile_max_x << " img_tile_max_y : " << img_tile_max_y << endl;

            cout << "FRACTAL_TILE: " << i << "," << j << " fractal_tile_min_x: " << fractal_tile_min_x << ", fractal_tile_min_y: "<< fractal_tile_min_y  \
                    << ", fractal_tile_max_x: " << fractal_tile_max_x << ", fractal_tile_max_y: " << fractal_tile_max_y << endl;

            // Fixes the bug!
            // if (i == j) continue; // ignore the tiles on the diagonal

            do_tile(&queue, &img, fractal_tile_min_x, fractal_tile_min_y, fractal_tile_max_x, fractal_tile_max_y, img_tile_min_x, img_tile_min_y, img_tile_max_x, img_tile_max_y, img_size_x, img_size_y);
        }
    }
    for (size_t i = 0; i < 8; i++) {
        std::shared_ptr<std::thread> thread = std::make_shared<std::thread>(render_fractal, &queue);
        threads.push_back(thread);
        // std::cout << "Made thread: " << i << std::endl;
    }

    do { 
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); 
        // std::cout << "queue size " << queue.size() << endl; 
    } while (queue.size() != 0);	

    queue.abort();

    for (auto thread : threads) {
        thread->join();
    }
    
    img.Write();
    return 0;
}
