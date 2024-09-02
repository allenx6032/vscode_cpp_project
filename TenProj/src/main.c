#include "glfw_tinycthread.h"
#include "glfw_getopt.h"
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <locale.h>
#include <math.h>
#include <tchar.h>
#include <assert.h>


#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "luabind.h"

static const char* titles[] =
{
    "Red",
    "Green",
    "Blue",
    "Yellow"
};

static const struct
{
    float r, g, b;
} colors[] =
{
    { 0.95f, 0.32f, 0.11f },
    { 0.50f, 0.80f, 0.16f },
    {   0.f, 0.68f, 0.94f },
    { 0.98f, 0.74f, 0.04f }
};
typedef struct
{
    GLFWwindow* window;
    const char* title;
    float r, g, b;
    thrd_t id;
} Thread;


enum Mode
{
    LIST_MODE,
    TEST_MODE
};

static void usage(void)
{
    printf("Usage: monitors [-t]\n");
    printf("       monitors -h\n");
    printf("Usage: msaa [-h] [-s SAMPLES]\n");
}

static const char* format_mode(const GLFWvidmode* mode)
{
    static char buffer[512];
    sprintf(buffer,
            "%i x %i x %i (%i %i %i) %i Hz",
            mode->width, mode->height,
            mode->redBits + mode->greenBits + mode->blueBits,
            mode->redBits, mode->greenBits, mode->blueBits,
            mode->refreshRate);
    buffer[sizeof(buffer) - 1] = '\0';
    return buffer;
}


typedef struct Joystick
{
    GLboolean present;
    char* name;
    float* axes;
    unsigned char* buttons;
    int axis_count;
    int button_count;
} Joystick;
static Joystick joysticks[GLFW_JOYSTICK_LAST - GLFW_JOYSTICK_1 + 1];
static int joystick_count = 0;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
        return;

    switch (key)
    {
        case GLFW_KEY_SPACE:
        {
            int xpos, ypos;
            glfwGetWindowPos(window, &xpos, &ypos);
            glfwSetWindowPos(window, xpos, ypos);
            break;
        }
        case GLFW_KEY_0:
        {
            glfwSetTime(0.0);
            break;
        }
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
    }
}

static volatile GLboolean running = GL_TRUE;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static int thread_main(void* data)
{
    const Thread* thread = data;

    glfwMakeContextCurrent(thread->window);
    glfwSwapInterval(1);

    while (running)
    {
        const float v = (float) fabs(sin(glfwGetTime() * 2.f));
        glClearColor(thread->r * v, thread->g * v, thread->b * v, 0.f);

        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(thread->window);
    }

    glfwMakeContextCurrent(NULL);
    return 0;
}


static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
     printf("Framebuffer resized to %ix%i\n", width, height);
    glViewport(0, 0, width, height);
}

static void draw_joystick(Joystick* j, int x, int y, int width, int height)
{
    int i;
    const int axis_height = 3 * height / 4;
    const int button_height = height / 4;
    if (j->axis_count)
    {
        const int axis_width = width / j->axis_count;
        for (i = 0;  i < j->axis_count;  i++)
        {
            float value = j->axes[i] / 2.f + 0.5f;
            glColor3f(0.3f, 0.3f, 0.3f);
            glRecti(x + i * axis_width,
                    y,
                    x + (i + 1) * axis_width,
                    y + axis_height);
            glColor3f(1.f, 1.f, 1.f);
            glRecti(x + i * axis_width,
                    y + (int) (value * (axis_height - 5)),
                    x + (i + 1) * axis_width,
                    y + 5 + (int) (value * (axis_height - 5)));
        }
    }
    if (j->button_count)
    {
        const int button_width = width / j->button_count;
        for (i = 0;  i < j->button_count;  i++)
        {
            if (j->buttons[i])
                glColor3f(1.f, 1.f, 1.f);
            else
                glColor3f(0.3f, 0.3f, 0.3f);

            glRecti(x + i * button_width,
                    y + axis_height,
                    x + (i + 1) * button_width,
                    y + axis_height + button_height);
        }
    }
}

static void draw_joysticks(GLFWwindow* window)
{
    int i, width, height, offset = 0;
    glfwGetFramebufferSize(window, &width, &height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.f, width, height, 0.f, 1.f, -1.f);
    glMatrixMode(GL_MODELVIEW);
    for (i = 0;  i < sizeof(joysticks) / sizeof(Joystick);  i++)
    {
        Joystick* j = joysticks + i;

        if (j->present)
        {
            draw_joystick(j,
                          0, offset * height / joystick_count,
                          width, height / joystick_count);
            offset++;
        }
    }
}

static void refresh_joysticks(void)
{
    int i;
    for (i = 0;  i < sizeof(joysticks) / sizeof(Joystick);  i++)
    {
        Joystick* j = joysticks + i;
        if (glfwJoystickPresent(GLFW_JOYSTICK_1 + i))
        {
            const float* axes;
            const unsigned char* buttons;
            int axis_count, button_count;
            free(j->name);
            j->name = strdup(glfwGetJoystickName(GLFW_JOYSTICK_1 + i));
            axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1 + i, &axis_count);
            if (axis_count != j->axis_count)
            {
                j->axis_count = axis_count;
                j->axes = realloc(j->axes, j->axis_count * sizeof(float));
            }
            memcpy(j->axes, axes, axis_count * sizeof(float));
            buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1 + i, &button_count);
            if (button_count != j->button_count)
            {
                j->button_count = button_count;
                j->buttons = realloc(j->buttons, j->button_count);
            }
            memcpy(j->buttons, buttons, button_count * sizeof(unsigned char));
            if (!j->present)
            {
                printf("Found joystick %i named \'%s\' with %i axes, %i buttons\n",
                       i + 1, j->name, j->axis_count, j->button_count);
                joystick_count++;
            }
            j->present = GL_TRUE;
        }
        else
        {
            if (j->present)
            {
                printf("Lost joystick %i named \'%s\'\n", i + 1, j->name);
                free(j->name);
                free(j->axes);
                free(j->buttons);
                memset(j, 0, sizeof(Joystick));

                joystick_count--;
            }
        }
    }
}

static void list_modes(GLFWmonitor* monitor)
{
    int count, x, y, widthMM, heightMM, i;
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    const GLFWvidmode* modes = glfwGetVideoModes(monitor, &count);
    glfwGetMonitorPos(monitor, &x, &y);
    glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);
    printf("Name: %s (%s)\n",
           glfwGetMonitorName(monitor),
           glfwGetPrimaryMonitor() == monitor ? "primary" : "secondary");
    printf("Current mode: %s\n", format_mode(mode));
    printf("Virtual position: %i %i\n", x, y);
    printf("Physical size: %i x %i mm (%0.2f dpi)\n",
           widthMM, heightMM, mode->width * 25.4f / widthMM);
    printf("Modes:\n");
    for (i = 0;  i < count;  i++)
    {
        printf("%3u: %s", (unsigned int) i, format_mode(modes + i));
        if (memcmp(mode, modes + i, sizeof(GLFWvidmode)) == 0)
            printf(" (current mode)");
        putchar('\n');
    }
}

static void test_modes(GLFWmonitor* monitor)
{
    int i, count;
    GLFWwindow* window;
    const GLFWvidmode* modes = glfwGetVideoModes(monitor, &count);
    for (i = 0;  i < count;  i++)
    {
        const GLFWvidmode* mode = modes + i;
        GLFWvidmode current;
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        printf("Testing mode %u on monitor %s: %s\n",
               (unsigned int) i,
               glfwGetMonitorName(monitor),
               format_mode(mode));
        window = glfwCreateWindow(mode->width, mode->height,
                                  "Video Mode Test",
                                  glfwGetPrimaryMonitor(),
                                  NULL);
        if (!window)
        {
            printf("Failed to enter mode %u: %s\n",
                   (unsigned int) i,
                   format_mode(mode));
            continue;
        }
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetKeyCallback(window, key_callback);
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);
        glfwSetTime(0.0);
        while (glfwGetTime() < 5.0)
        {
            glClear(GL_COLOR_BUFFER_BIT);
            glfwSwapBuffers(window);
            glfwPollEvents();
            if (glfwWindowShouldClose(window))
            {
                printf("User terminated program\n");

                glfwTerminate();
                exit(EXIT_SUCCESS);
            }
        }
        glGetIntegerv(GL_RED_BITS, &current.redBits);
        glGetIntegerv(GL_GREEN_BITS, &current.greenBits);
        glGetIntegerv(GL_BLUE_BITS, &current.blueBits);
        glfwGetWindowSize(window, &current.width, &current.height);
        if (current.redBits != mode->redBits ||
            current.greenBits != mode->greenBits ||
            current.blueBits != mode->blueBits)
        {
            printf("*** Color bit mismatch: (%i %i %i) instead of (%i %i %i)\n",
                   current.redBits, current.greenBits, current.blueBits,
                   mode->redBits, mode->greenBits, mode->blueBits);
        }
        if (current.width != mode->width || current.height != mode->height)
        {
            printf("*** Size mismatch: %ix%i instead of %ix%i\n",
                   current.width, current.height,
                   mode->width, mode->height);
        }
        printf("Closing window\n");
        glfwDestroyWindow(window);
        window = NULL;
        glfwPollEvents();
    }
}

static unsigned int counter = 0;
typedef struct
{
    GLFWwindow* window;
    int number;
    int closeable;
} Slot;
static void event_usage(void)
{
    printf("Usage: events [-f] [-h] [-n WINDOWS]\n");
    printf("Options:\n");
    printf("  -f use full screen\n");
    printf("  -h show this help\n");
    printf("  -n the number of windows to create\n");
}


static const char* get_key_name(int key)
{
    switch (key)
    {
        // Printable keys
        case GLFW_KEY_A:            return "A";
        case GLFW_KEY_B:            return "B";
        case GLFW_KEY_C:            return "C";
        case GLFW_KEY_D:            return "D";
        case GLFW_KEY_E:            return "E";
        case GLFW_KEY_F:            return "F";
        case GLFW_KEY_G:            return "G";
        case GLFW_KEY_H:            return "H";
        case GLFW_KEY_I:            return "I";
        case GLFW_KEY_J:            return "J";
        case GLFW_KEY_K:            return "K";
        case GLFW_KEY_L:            return "L";
        case GLFW_KEY_M:            return "M";
        case GLFW_KEY_N:            return "N";
        case GLFW_KEY_O:            return "O";
        case GLFW_KEY_P:            return "P";
        case GLFW_KEY_Q:            return "Q";
        case GLFW_KEY_R:            return "R";
        case GLFW_KEY_S:            return "S";
        case GLFW_KEY_T:            return "T";
        case GLFW_KEY_U:            return "U";
        case GLFW_KEY_V:            return "V";
        case GLFW_KEY_W:            return "W";
        case GLFW_KEY_X:            return "X";
        case GLFW_KEY_Y:            return "Y";
        case GLFW_KEY_Z:            return "Z";
        case GLFW_KEY_1:            return "1";
        case GLFW_KEY_2:            return "2";
        case GLFW_KEY_3:            return "3";
        case GLFW_KEY_4:            return "4";
        case GLFW_KEY_5:            return "5";
        case GLFW_KEY_6:            return "6";
        case GLFW_KEY_7:            return "7";
        case GLFW_KEY_8:            return "8";
        case GLFW_KEY_9:            return "9";
        case GLFW_KEY_0:            return "0";
        case GLFW_KEY_SPACE:        return "SPACE";
        case GLFW_KEY_MINUS:        return "MINUS";
        case GLFW_KEY_EQUAL:        return "EQUAL";
        case GLFW_KEY_LEFT_BRACKET: return "LEFT BRACKET";
        case GLFW_KEY_RIGHT_BRACKET: return "RIGHT BRACKET";
        case GLFW_KEY_BACKSLASH:    return "BACKSLASH";
        case GLFW_KEY_SEMICOLON:    return "SEMICOLON";
        case GLFW_KEY_APOSTROPHE:   return "APOSTROPHE";
        case GLFW_KEY_GRAVE_ACCENT: return "GRAVE ACCENT";
        case GLFW_KEY_COMMA:        return "COMMA";
        case GLFW_KEY_PERIOD:       return "PERIOD";
        case GLFW_KEY_SLASH:        return "SLASH";
        case GLFW_KEY_WORLD_1:      return "WORLD 1";
        case GLFW_KEY_WORLD_2:      return "WORLD 2";

        // Function keys
        case GLFW_KEY_ESCAPE:       return "ESCAPE";
        case GLFW_KEY_F1:           return "F1";
        case GLFW_KEY_F2:           return "F2";
        case GLFW_KEY_F3:           return "F3";
        case GLFW_KEY_F4:           return "F4";
        case GLFW_KEY_F5:           return "F5";
        case GLFW_KEY_F6:           return "F6";
        case GLFW_KEY_F7:           return "F7";
        case GLFW_KEY_F8:           return "F8";
        case GLFW_KEY_F9:           return "F9";
        case GLFW_KEY_F10:          return "F10";
        case GLFW_KEY_F11:          return "F11";
        case GLFW_KEY_F12:          return "F12";
        case GLFW_KEY_F13:          return "F13";
        case GLFW_KEY_F14:          return "F14";
        case GLFW_KEY_F15:          return "F15";
        case GLFW_KEY_F16:          return "F16";
        case GLFW_KEY_F17:          return "F17";
        case GLFW_KEY_F18:          return "F18";
        case GLFW_KEY_F19:          return "F19";
        case GLFW_KEY_F20:          return "F20";
        case GLFW_KEY_F21:          return "F21";
        case GLFW_KEY_F22:          return "F22";
        case GLFW_KEY_F23:          return "F23";
        case GLFW_KEY_F24:          return "F24";
        case GLFW_KEY_F25:          return "F25";
        case GLFW_KEY_UP:           return "UP";
        case GLFW_KEY_DOWN:         return "DOWN";
        case GLFW_KEY_LEFT:         return "LEFT";
        case GLFW_KEY_RIGHT:        return "RIGHT";
        case GLFW_KEY_LEFT_SHIFT:   return "LEFT SHIFT";
        case GLFW_KEY_RIGHT_SHIFT:  return "RIGHT SHIFT";
        case GLFW_KEY_LEFT_CONTROL: return "LEFT CONTROL";
        case GLFW_KEY_RIGHT_CONTROL: return "RIGHT CONTROL";
        case GLFW_KEY_LEFT_ALT:     return "LEFT ALT";
        case GLFW_KEY_RIGHT_ALT:    return "RIGHT ALT";
        case GLFW_KEY_TAB:          return "TAB";
        case GLFW_KEY_ENTER:        return "ENTER";
        case GLFW_KEY_BACKSPACE:    return "BACKSPACE";
        case GLFW_KEY_INSERT:       return "INSERT";
        case GLFW_KEY_DELETE:       return "DELETE";
        case GLFW_KEY_PAGE_UP:      return "PAGE UP";
        case GLFW_KEY_PAGE_DOWN:    return "PAGE DOWN";
        case GLFW_KEY_HOME:         return "HOME";
        case GLFW_KEY_END:          return "END";
        case GLFW_KEY_KP_0:         return "KEYPAD 0";
        case GLFW_KEY_KP_1:         return "KEYPAD 1";
        case GLFW_KEY_KP_2:         return "KEYPAD 2";
        case GLFW_KEY_KP_3:         return "KEYPAD 3";
        case GLFW_KEY_KP_4:         return "KEYPAD 4";
        case GLFW_KEY_KP_5:         return "KEYPAD 5";
        case GLFW_KEY_KP_6:         return "KEYPAD 6";
        case GLFW_KEY_KP_7:         return "KEYPAD 7";
        case GLFW_KEY_KP_8:         return "KEYPAD 8";
        case GLFW_KEY_KP_9:         return "KEYPAD 9";
        case GLFW_KEY_KP_DIVIDE:    return "KEYPAD DIVIDE";
        case GLFW_KEY_KP_MULTIPLY:  return "KEYPAD MULTPLY";
        case GLFW_KEY_KP_SUBTRACT:  return "KEYPAD SUBTRACT";
        case GLFW_KEY_KP_ADD:       return "KEYPAD ADD";
        case GLFW_KEY_KP_DECIMAL:   return "KEYPAD DECIMAL";
        case GLFW_KEY_KP_EQUAL:     return "KEYPAD EQUAL";
        case GLFW_KEY_KP_ENTER:     return "KEYPAD ENTER";
        case GLFW_KEY_PRINT_SCREEN: return "PRINT SCREEN";
        case GLFW_KEY_NUM_LOCK:     return "NUM LOCK";
        case GLFW_KEY_CAPS_LOCK:    return "CAPS LOCK";
        case GLFW_KEY_SCROLL_LOCK:  return "SCROLL LOCK";
        case GLFW_KEY_PAUSE:        return "PAUSE";
        case GLFW_KEY_LEFT_SUPER:   return "LEFT SUPER";
        case GLFW_KEY_RIGHT_SUPER:  return "RIGHT SUPER";
        case GLFW_KEY_MENU:         return "MENU";

        default:                    return "UNKNOWN";
    }
}

static const char* get_action_name(int action)
{
    switch (action)
    {
        case GLFW_PRESS:
            return "pressed";
        case GLFW_RELEASE:
            return "released";
        case GLFW_REPEAT:
            return "repeated";
    }

    return "caused unknown action";
}

static const char* get_button_name(int button)
{
    switch (button)
    {
        case GLFW_MOUSE_BUTTON_LEFT:
            return "left";
        case GLFW_MOUSE_BUTTON_RIGHT:
            return "right";
        case GLFW_MOUSE_BUTTON_MIDDLE:
            return "middle";
        default:
        {
            static char name[16];
            sprintf(name, "%i", button);
            return name;
        }
    }
}

static const char* get_mods_name(int mods)
{
    static char name[512];

    if (mods == 0)
        return " no mods";

    name[0] = '\0';

    if (mods & GLFW_MOD_SHIFT)
        strcat(name, " shift");
    if (mods & GLFW_MOD_CONTROL)
        strcat(name, " control");
    if (mods & GLFW_MOD_ALT)
        strcat(name, " alt");
    if (mods & GLFW_MOD_SUPER)
        strcat(name, " super");

    return name;
}

static const char* get_character_string(int codepoint)
{
    // This assumes UTF-8, which is stupid
    static char result[6 + 1];

    int length = wctomb(result, codepoint);
    if (length == -1)
        length = 0;

    result[length] = '\0';
    return result;
}

static void window_pos_callback(GLFWwindow* window, int x, int y)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Window position: %i %i\n",
           counter++, slot->number, glfwGetTime(), x, y);
}

static void window_size_callback(GLFWwindow* window, int width, int height)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Window size: %i %i\n",
           counter++, slot->number, glfwGetTime(), width, height);
}

static void sframebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Framebuffer size: %i %i\n",
           counter++, slot->number, glfwGetTime(), width, height);
    glViewport(0, 0, width, height);
}


static void window_close_callback(GLFWwindow* window)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Window close\n",
           counter++, slot->number, glfwGetTime());
    glfwSetWindowShouldClose(window, slot->closeable);
}

static void window_refresh_callback(GLFWwindow* window)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Window refresh\n",
           counter++, slot->number, glfwGetTime());
    glfwMakeContextCurrent(window);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
}

static void window_focus_callback(GLFWwindow* window, int focused)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Window %s\n",
           counter++, slot->number, glfwGetTime(),
           focused ? "focused" : "defocused");
}

static void window_iconify_callback(GLFWwindow* window, int iconified)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Window was %s\n",
           counter++, slot->number, glfwGetTime(),
           iconified ? "iconified" : "restored");
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Mouse button %i (%s) (with%s) was %s\n",
           counter++, slot->number, glfwGetTime(), button,
           get_button_name(button),
           get_mods_name(mods),
           get_action_name(action));
}

static void cursor_position_callback(GLFWwindow* window, double x, double y)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Cursor position: %f %f\n",
           counter++, slot->number, glfwGetTime(), x, y);
}

static void cursor_enter_callback(GLFWwindow* window, int entered)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Cursor %s window\n",
           counter++, slot->number, glfwGetTime(),
           entered ? "entered" : "left");
}

static void scroll_callback(GLFWwindow* window, double x, double y)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Scroll: %0.3f %0.3f\n",
           counter++, slot->number, glfwGetTime(), x, y);
}

static void skey_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Key 0x%04x Scancode 0x%04x (%s) (with%s) was %s\n",
           counter++, slot->number, glfwGetTime(), key, scancode,
           get_key_name(key),
           get_mods_name(mods),
           get_action_name(action));
    if (action != GLFW_PRESS)
        return;
    switch (key)
    {
        case GLFW_KEY_C:
        {
            slot->closeable = !slot->closeable;
            printf("(( closing %s ))\n", slot->closeable ? "enabled" : "disabled");
            break;
        }
    }
}

static void char_callback(GLFWwindow* window, unsigned int codepoint)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Character 0x%08x (%s) input\n",
           counter++, slot->number, glfwGetTime(), codepoint,
           get_character_string(codepoint));
}

static void char_mods_callback(GLFWwindow* window, unsigned int codepoint, int mods)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Character 0x%08x (%s) with modifiers (with%s) input\n",
            counter++, slot->number, glfwGetTime(), codepoint,
            get_character_string(codepoint),
            get_mods_name(mods));
}

static void drop_callback(GLFWwindow* window, int count, const char** paths)
{
    int i;
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Drop input\n",
           counter++, slot->number, glfwGetTime());
    for (i = 0;  i < count;  i++)
        printf("  %i: \"%s\"\n", i, paths[i]);
}

static void monitor_callback(GLFWmonitor* monitor, int event)
{
    if (event == GLFW_CONNECTED)
    {
        int x, y, widthMM, heightMM;
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwGetMonitorPos(monitor, &x, &y);
        glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);
        printf("%08x at %0.3f: Monitor %s (%ix%i at %ix%i, %ix%i mm) was connected\n",
               counter++,
               glfwGetTime(),
               glfwGetMonitorName(monitor),
               mode->width, mode->height,
               x, y,
               widthMM, heightMM);
    }
    else
    {
        printf("%08x at %0.3f: Monitor %s was disconnected\n",
               counter++,
               glfwGetTime(),
               glfwGetMonitorName(monitor));
    }
}

#define CURSOR_FRAME_COUNT 60
static double cursor_x;
static double cursor_y;
static int swap_interval = 1;
static GLboolean wait_events = GL_FALSE;
static GLboolean animate_cursor = GL_FALSE;
static GLboolean track_cursor = GL_FALSE;
static GLFWcursor* standard_cursors[6];

static float star(int x, int y, float t)
{
    const float c = 64 / 2.f;
    const float i = (0.25f * (float) sin(2.f * M_PI * t) + 0.75f);
    const float k = 64 * 0.046875f * i;
    const float dist = (float) sqrt((x - c) * (x - c) + (y - c) * (y - c));
    const float salpha = 1.f - dist / c;
    const float xalpha = (float) x == c ? c : k / (float) fabs(x - c);
    const float yalpha = (float) y == c ? c : k / (float) fabs(y - c);
    return (float) fmax(0.f, fmin(1.f, i * salpha * 0.2f + salpha * xalpha * yalpha));
}

static GLFWcursor* create_cursor_frame(float t)
{
    int i = 0, x, y;
    unsigned char buffer[64 * 64 * 4];
    const GLFWimage image = { 64, 64, buffer };
    for (y = 0;  y < image.width;  y++)
    {
        for (x = 0;  x < image.height;  x++)
        {
            buffer[i++] = 255;
            buffer[i++] = 255;
            buffer[i++] = 255;
            buffer[i++] = (unsigned char) (255 * star(x, y, t));
        }
    }
    return glfwCreateCursor(&image, image.width / 2, image.height / 2);
}

static void ccursor_position_callback(GLFWwindow* window, double x, double y)
{
    printf("%0.3f: Cursor position: %f %f (%+f %+f)\n",
           glfwGetTime(),
           x, y, x - cursor_x, y - cursor_y);
    cursor_x = x;
    cursor_y = y;
}

static void ckey_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
        return;
    switch (key)
    {
        case GLFW_KEY_A:
        {
            animate_cursor = !animate_cursor;
            if (!animate_cursor)
                glfwSetCursor(window, NULL);

            break;
        }
        case GLFW_KEY_ESCAPE:
        {
            if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
            {
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            }
            /* FALLTHROUGH */
        }
        case GLFW_KEY_N:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            printf("(( cursor is normal ))\n");
            break;
        case GLFW_KEY_D:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            printf("(( cursor is disabled ))\n");
            break;
        case GLFW_KEY_H:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            printf("(( cursor is hidden ))\n");
            break;
        case GLFW_KEY_SPACE:
            swap_interval = 1 - swap_interval;
            printf("(( swap interval: %i ))\n", swap_interval);
            glfwSwapInterval(swap_interval);
            break;
        case GLFW_KEY_W:
            wait_events = !wait_events;
            printf("(( %sing for events ))\n", wait_events ? "wait" : "poll");
            break;
        case GLFW_KEY_T:
            track_cursor = !track_cursor;
            break;
        case GLFW_KEY_0:
            glfwSetCursor(window, NULL);
            break;
        case GLFW_KEY_1:
            glfwSetCursor(window, standard_cursors[0]);
            break;
        case GLFW_KEY_2:
            glfwSetCursor(window, standard_cursors[1]);
            break;
        case GLFW_KEY_3:
            glfwSetCursor(window, standard_cursors[2]);
            break;
        case GLFW_KEY_4:
            glfwSetCursor(window, standard_cursors[3]);
            break;
        case GLFW_KEY_5:
            glfwSetCursor(window, standard_cursors[4]);
            break;
        case GLFW_KEY_6:
            glfwSetCursor(window, standard_cursors[5]);
            break;
    }
}


#if defined(__APPLE__)
 #define MODIFIER GLFW_MOD_SUPER
#else
 #define MODIFIER GLFW_MOD_CONTROL
#endif

static void clipboard_usage(void)
{
    printf("Usage: clipboard [-h]\n");
}


static void cb_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
        return;
    switch (key)
    {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_V:
            if (mods == MODIFIER)
            {
                const char* string;
                string = glfwGetClipboardString(window);
                if (string)
                    printf("Clipboard contains \"%s\"\n", string);
                else
                    printf("Clipboard does not contain a string\n");
            }
            break;
        case GLFW_KEY_C:
            if (mods == MODIFIER)
            {
                const char* string = "Hello GLFW World!";
                glfwSetClipboardString(window, string);
                printf("Setting clipboard to \"%s\"\n", string);
            }
            break;
    }
}

int main(int argc, char** argv)
{

    int ch, samples = 4,  mode = LIST_MODE;
    int i, result;
    int count;

    // GLFWmonitor** monitors;
    // while ((ch = getopt(argc, argv, "th")) != -1)
    // {
    //     switch (ch)
    //     {
    //         case 'h':
    //             usage();
    //             exit(EXIT_SUCCESS);
    //         case 't':
    //             mode = TEST_MODE;
    //             break;
    //         default:
    //             usage();
    //             exit(EXIT_FAILURE);
    //     }
    // }
    // glfwSetErrorCallback(error_callback);
    // if (!glfwInit())
    //     exit(EXIT_FAILURE);
    // monitors = glfwGetMonitors(&count);
    // for (i = 0;  i < count;  i++)
    // {
    //     if (mode == LIST_MODE)
    //         list_modes(monitors[i]);
    //     else if (mode == TEST_MODE)
    //         test_modes(monitors[i]);
    // }
    // glfwTerminate();

    // GLFWwindow* msaa_window;
    // while ((ch = getopt(argc, argv, "hs:")) != -1)
    // {
    //     switch (ch)
    //     {
    //         case 'h':
    //             usage();
    //             exit(EXIT_SUCCESS);
    //         case 's':
    //             samples = atoi(optarg);
    //             break;
    //         default:
    //             usage();
    //             exit(EXIT_FAILURE);
    //     }
    // }
    // glfwSetErrorCallback(error_callback);
    // if (!glfwInit())
    //     exit(EXIT_FAILURE);
    // if (samples)
    //     printf("Requesting MSAA with %i samples\n", samples);
    // else
    //     printf("Requesting that MSAA not be available\n");
    // glfwWindowHint(GLFW_SAMPLES, samples);
    // glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    // msaa_window = glfwCreateWindow(800, 400, "Aliasing Detector", NULL, NULL);
    // if (!msaa_window)
    // {
    //     glfwTerminate();
    //     exit(EXIT_FAILURE);
    // }
    // glfwSetKeyCallback(msaa_window, key_callback);
    // glfwSetFramebufferSizeCallback(msaa_window, framebuffer_size_callback);
    // glfwMakeContextCurrent(msaa_window);
    // glfwSwapInterval(1);
    // if (!glfwExtensionSupported("GL_ARB_multisample"))
    // {
    //     printf("GL_ARB_multisample extension not supported\n");
    //     glfwTerminate();
    //     exit(EXIT_FAILURE);
    // }
    // glfwShowWindow(msaa_window);
    // glGetIntegerv(GL_SAMPLES_ARB, &samples);
    // if (samples)
    //     printf("Context reports MSAA is available with %i samples\n", samples);
    // else
    //     printf("Context reports MSAA is unavailable\n");
    // glMatrixMode(GL_PROJECTION);
    // glOrtho(0.f, 1.f, 0.f, 0.5f, 0.f, 1.f);
    // glMatrixMode(GL_MODELVIEW);
    // while (!glfwWindowShouldClose(msaa_window))
    // {
    //     GLfloat time = (GLfloat) glfwGetTime();
    //     glClear(GL_COLOR_BUFFER_BIT);
    //     glLoadIdentity();
    //     glTranslatef(0.25f, 0.25f, 0.f);
    //     glRotatef(time, 0.f, 0.f, 1.f);
    //     glDisable(GL_MULTISAMPLE_ARB);
    //     glRectf(-0.15f, -0.15f, 0.15f, 0.15f);
    //     glLoadIdentity();
    //     glTranslatef(0.75f, 0.25f, 0.f);
    //     glRotatef(time, 0.f, 0.f, 1.f);
    //     glEnable(GL_MULTISAMPLE_ARB);
    //     glRectf(-0.15f, -0.15f, 0.15f, 0.15f);
    //     glfwSwapBuffers(window);
    //     glfwPollEvents();
    // }
    // glfwTerminate();

    Slot* slots;
    GLFWmonitor* monitor = NULL;
    int swidth, sheight, scount = 1;
    setlocale(LC_ALL, "");

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    printf("Library initialized\n");
    glfwSetMonitorCallback(monitor_callback);
    while ((ch = getopt(argc, argv, "hfn:")) != -1)
    {
        switch (ch)
        {
            case 'h':
                usage();
                exit(EXIT_SUCCESS);
            case 'f':
                monitor = glfwGetPrimaryMonitor();
                break;
            case 'n':
                scount = (int) strtol(optarg, NULL, 10);
                break;
            default:
                usage();
                exit(EXIT_FAILURE);
        }
    }
    if (monitor)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        swidth = mode->width;
        sheight = mode->height;
    }
    else
    {
        swidth  = 640;
        sheight = 480;
    }
    if (!scount)
    {
        fprintf(stderr, "Invalid user\n");
        exit(EXIT_FAILURE);
    }
    slots = calloc(scount, sizeof(Slot));
    for (i = 0;  i < scount;  i++)
    {
        char title[128];
        slots[i].closeable = GL_TRUE;
        slots[i].number = i + 1;
        sprintf(title, "Event Linter (Window %i)", slots[i].number);
        if (monitor)
        {
            printf("Creating full screen window %i (%ix%i on %s)\n",
                   slots[i].number,
                   swidth, sheight,
                   glfwGetMonitorName(monitor));
        }
        else
        {
            printf("Creating windowed mode window %i (%ix%i)\n",
                   slots[i].number,
                   swidth, sheight);
        }
        slots[i].window = glfwCreateWindow(swidth, sheight, title, monitor, NULL);
        if (!slots[i].window)
        {
            free(slots);
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        glfwSetWindowUserPointer(slots[i].window, slots + i);
        glfwSetWindowPosCallback(slots[i].window, window_pos_callback);
        glfwSetWindowSizeCallback(slots[i].window, window_size_callback);
        glfwSetFramebufferSizeCallback(slots[i].window, sframebuffer_size_callback);
        glfwSetWindowCloseCallback(slots[i].window, window_close_callback);
        glfwSetWindowRefreshCallback(slots[i].window, window_refresh_callback);
        glfwSetWindowFocusCallback(slots[i].window, window_focus_callback);
        glfwSetWindowIconifyCallback(slots[i].window, window_iconify_callback);
        glfwSetMouseButtonCallback(slots[i].window, mouse_button_callback);
        glfwSetCursorPosCallback(slots[i].window, cursor_position_callback);
        glfwSetCursorEnterCallback(slots[i].window, cursor_enter_callback);
        glfwSetScrollCallback(slots[i].window, scroll_callback);
        glfwSetKeyCallback(slots[i].window, skey_callback);
        glfwSetCharCallback(slots[i].window, char_callback);
        glfwSetCharModsCallback(slots[i].window, char_mods_callback);
        glfwSetDropCallback(slots[i].window, drop_callback);
        glfwMakeContextCurrent(slots[i].window);
        glfwSwapInterval(1);
    }
    printf("Main loop starting\n");
    for (;;)
    {
        for (i = 0;  i < scount;  i++)
        {
            if (glfwWindowShouldClose(slots[i].window))
                break;
        }
        if (i < scount)
            break;
        glfwWaitEvents();
        fflush(stdout);
    }
    free(slots);
    glfwTerminate();


    GLFWwindow* joystick_window;
    memset(joysticks, 0, sizeof(joysticks));

    Thread threads[] =
    {
        { NULL, "Red", 1.f, 0.f, 0.f, 0 },
        { NULL, "Green", 0.f, 1.f, 0.f, 0 },
        { NULL, "Blue", 0.f, 0.f, 1.f, 0 },
        { NULL, "YELLOW", 0.5f, 0.5f, 1.f, 0 }
    };
    const int mycount = sizeof(threads) / sizeof(Thread);

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    joystick_window = glfwCreateWindow(640, 480, "Joystick Test", NULL, NULL);
    if (!joystick_window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetFramebufferSizeCallback(joystick_window, framebuffer_size_callback);
    glfwMakeContextCurrent(joystick_window);
    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(joystick_window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        refresh_joysticks();
        draw_joysticks(joystick_window);
        glfwSwapBuffers(joystick_window);
        glfwPollEvents();
    }
    glfwTerminate();



    GLFWwindow* cursor_window;
    GLFWcursor* star_cursors[CURSOR_FRAME_COUNT];
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    for (i = 0;  i < CURSOR_FRAME_COUNT;  i++)
    {
        star_cursors[i] = create_cursor_frame(i / (float) CURSOR_FRAME_COUNT);
        if (!star_cursors[i])
        {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
    }
    for (i = 0;  i < sizeof(standard_cursors) / sizeof(standard_cursors[0]);  i++)
    {
        const int shapes[] = {
            GLFW_ARROW_CURSOR,
            GLFW_IBEAM_CURSOR,
            GLFW_CROSSHAIR_CURSOR,
            GLFW_HAND_CURSOR,
            GLFW_HRESIZE_CURSOR,
            GLFW_VRESIZE_CURSOR
        };
        standard_cursors[i] = glfwCreateStandardCursor(shapes[i]);
        if (!standard_cursors[i])
        {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
    }
    cursor_window = glfwCreateWindow(640, 480, "Cursor Test", NULL, NULL);
    if (!cursor_window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(cursor_window);
    glfwGetCursorPos(cursor_window, &cursor_x, &cursor_y);
    printf("Cursor position: %f %f\n", cursor_x, cursor_y);
    glfwSetCursorPosCallback(cursor_window, ccursor_position_callback);
    glfwSetKeyCallback(cursor_window, ckey_callback);
    while (!glfwWindowShouldClose(cursor_window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        if (track_cursor)
        {
            int wnd_width, wnd_height, fb_width, fb_height;
            float scale;
            glfwGetWindowSize(cursor_window, &wnd_width, &wnd_height);
            glfwGetFramebufferSize(cursor_window, &fb_width, &fb_height);
            scale = (float) fb_width / (float) wnd_width;
            glViewport(0, 0, fb_width, fb_height);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0.f, fb_width, 0.f, fb_height, 0.f, 1.f);
            glBegin(GL_LINES);
            glVertex2f(0.f, (GLfloat) (fb_height - cursor_y * scale));
            glVertex2f((GLfloat) fb_width, (GLfloat) (fb_height - cursor_y * scale));
            glVertex2f((GLfloat) cursor_x * scale, 0.f);
            glVertex2f((GLfloat) cursor_x * scale, (GLfloat) fb_height);
            glEnd();
        }
        glfwSwapBuffers(cursor_window);
        if (animate_cursor)
        {
            const int i = (int) (glfwGetTime() * 30.0) % CURSOR_FRAME_COUNT;
            glfwSetCursor(cursor_window, star_cursors[i]);
        }
        if (wait_events)
            glfwWaitEvents();
        else
            glfwPollEvents();
        // Workaround for an issue with msvcrt and mintty
        fflush(stdout);
    }
    glfwDestroyWindow(cursor_window);
    for (i = 0;  i < CURSOR_FRAME_COUNT;  i++)
        glfwDestroyCursor(star_cursors[i]);
    for (i = 0;  i < sizeof(standard_cursors) / sizeof(standard_cursors[0]);  i++)
        glfwDestroyCursor(standard_cursors[i]);
    glfwTerminate();



    GLFWwindow* clipboard_window;
    while ((ch = getopt(argc, argv, "h")) != -1)
    {
        switch (ch)
        {
            case 'h':
                clipboard_usage();
                exit(EXIT_SUCCESS);

            default:
                clipboard_usage();
                exit(EXIT_FAILURE);
        }
    }
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }
    clipboard_window = glfwCreateWindow(200, 200, "Clipboard Test", NULL, NULL);
    if (!clipboard_window)
    {
        glfwTerminate();

        fprintf(stderr, "Failed to open GLFW window\n");
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(clipboard_window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(clipboard_window, cb_key_callback);
    glfwSetFramebufferSizeCallback(clipboard_window, framebuffer_size_callback);
    glMatrixMode(GL_PROJECTION);
    glOrtho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);
    glMatrixMode(GL_MODELVIEW);
    glClearColor(0.5f, 0.5f, 0.5f, 0);
    while (!glfwWindowShouldClose(clipboard_window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(0.8f, 0.2f, 0.4f);
        glRectf(-0.5f, -0.5f, 0.5f, 0.5f);
        glfwSwapBuffers(clipboard_window);
        glfwWaitEvents();
    }
    glfwTerminate();


    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    glfwWindowHint(GLFW_DECORATED, GL_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    for (i = 0;  i < mycount;  i++)
    {
        int left, top, right, bottom;
        threads[i].window = glfwCreateWindow(200, 200,
                                             threads[i].title,
                                             NULL, NULL);
        if (!threads[i].window)
        {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        glfwSetKeyCallback(threads[i].window, key_callback);
        // glfwMakeContextCurrent(threads[i].window);
        // glClearColor(colors[i].r, colors[i].g, colors[i].b, 1.f);
        glfwSetWindowPos(threads[i].window, 200 + 250 * i, 200);
        glfwShowWindow(threads[i].window);
        glfwGetWindowFrameSize(threads[i].window, &left, &top, &right, &bottom);
        glfwSetWindowPos(threads[i].window,
                         100 + (i & 1) * (200 + left + right),
                         100 + (i >> 1) * (200 + top + bottom));
        if (thrd_create(&threads[i].id, thread_main, threads + i) !=
            thrd_success)
        {
            fprintf(stderr, "Failed to create secondary thread\n");

            glfwTerminate();
            exit(EXIT_FAILURE);
        }
    }

    while (running)
    {
        // glfwWaitEvents();
        for (i = 0;  i < mycount;  i++)
        {
            glfwMakeContextCurrent(threads[i].window);
            glClear(GL_COLOR_BUFFER_BIT);
            glfwSwapBuffers(threads[i].window);
            if (glfwWindowShouldClose(threads[i].window))
                running = GL_FALSE;
        }
        // glfwPollEvents();
    }
    for (i = 0;  i < mycount;  i++)
        glfwHideWindow(threads[i].window);
    for (i = 0;  i < mycount;  i++)
        thrd_join(threads[i].id, &result);
    exit(EXIT_SUCCESS);
}

