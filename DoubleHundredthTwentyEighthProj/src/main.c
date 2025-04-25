#include <faur.h>

void f_main(void)
{
    static struct {
        int x, y;
        FButton *up, *down, *left, *right;
    } context;

    F_STATE_INIT
    {
        context.x = f_screen_sizeGetWidth() / 2;
        context.y = f_screen_sizeGetHeight() / 2;

        context.up = f_button_new();
        f_button_bindKey(context.up, F_KEY_UP);
        f_button_bindButton(context.up, F_BUTTON_UP);

        context.down = f_button_new();
        f_button_bindKey(context.down, F_KEY_DOWN);
        f_button_bindButton(context.down, F_BUTTON_DOWN);

        context.left = f_button_new();
        f_button_bindKey(context.left, F_KEY_LEFT);
        f_button_bindButton(context.left, F_BUTTON_LEFT);

        context.right = f_button_new();
        f_button_bindKey(context.right, F_KEY_RIGHT);
        f_button_bindButton(context.right, F_BUTTON_RIGHT);
    }

    F_STATE_TICK
    {
        if(f_button_pressGet(context.up)) context.y--;
        if(f_button_pressGet(context.down)) context.y++;
        if(f_button_pressGet(context.left)) context.x--;
        if(f_button_pressGet(context.right)) context.x++;
    }

    F_STATE_DRAW
    {
        f_color_colorSetHex(0xaaff88);
        f_draw_fill();

        f_color_colorSetHex(0xffaa44);
        f_draw_rectangle(context.x - 40, context.y - 40, 80, 80);
    }

    F_STATE_FREE
    {
        f_button_free(context.up);
        f_button_free(context.down);
        f_button_free(context.left);
        f_button_free(context.right);
    }
}