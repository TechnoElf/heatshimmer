#include "render/hid.h"
#include "render/renderer.h"

int main(int argc, char* argv[]) {
    hs::ren::Hid hid = hs::ren::Hid();
    hs::ren::Renderer renderer = hs::ren::Renderer(hid);

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

        renderer.draw(hid.is_pressed());
    }

    return 0;
}
