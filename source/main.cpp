#include "render/hid.h"
#include "render/renderer.h"

int main(int argc, char* argv[]) {
    hs::ren::Hid hid = hs::ren::Hid();
    hs::ren::Renderer renderer = hs::ren::Renderer(hid);
    hs::str::World world = hs::str::World();

    hs::str::Object obj1 = hs::str::Object(hs::ren::Model::init(
        {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
        },
        {
            0, 1, 2
        }
    ));
    obj1.position.z = 1;
    obj1.position.x = 0.1;
    obj1.rotation.z = 0.1;

    world.add_object(std::move(obj1));

    hs::str::Object obj2 = hs::str::Object(hs::ren::Model::init(
        {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
        },
        {
            0, 2, 3
        }
    ));
    obj2.position.z = 1;
    world.add_object(std::move(obj2));

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
