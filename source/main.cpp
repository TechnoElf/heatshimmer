#include "render/hid.h"
#include "render/renderer.h"

int main(int argc, char* argv[]) {
    hs::ren::Hid hid = hs::ren::Hid();
    hs::ren::Renderer renderer = hs::ren::Renderer(hid);
    hs::str::World world = hs::str::World();

    world.add_object(hs::str::Object(hs::ren::Model::init(
        {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
        },
        {
            0, 1, 2
        }
    )));

    world.add_object(hs::str::Object(hs::ren::Model::init(
        {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
        },
        {
            0, 2, 3
        }
    )));

    renderer.sync(world);

    bool quit = false;
    while (!quit) {
        switch (hid.poll()) {
            case hs::ren::HID_QUIT: {
                quit = true;
                break;
            }
            case hs::ren::HID_NONE: {
                break;
            }
        }

        renderer.draw(hid.is_pressed(), world, hid);
    }

    return 0;
}
