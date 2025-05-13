#include "DungeonProwler_PCH.h"
#include "DungeonProwler_Game.h"


// http://www.concentric.net/~Ttwang/tech/inthash.htm
unsigned int mix(unsigned long a, unsigned long b, unsigned long c)
{
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return static_cast<unsigned int>(c);
}


// Entry point of the application.
int main()
{
    // Set a random seed.
    unsigned int seed = mix(clock(), static_cast<unsigned long>(time(NULL)), 1024);
    srand(seed);

    // Create the main game object.
    //sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH,WINDOW_HEIGHT),"window Title");
    //window.setFramerateLimit(60);//设置帧率
    //游戏循环
    //while(window.isOpen())
    //{
   	//1.事件处理
    //sf::Event event;
    //while(window.pollEvent(event))
    //{
    // switch(event.type)
    // {
    // //窗体事件
    // case sf::Event::Closed:
    //     window.close();
    //     break;
    // case sf::Event::Resized:
    //     std::cout <<" height:" << event.size.height   <<" width:" << event.size.width << std::endl;
    //     break;
    // case sf::Event::LostFocus:
    //     std::cout << "失去焦点" << std::endl;
    //     break;
    // case sf::Event::GainedFocus:
    //     std::cout << "获得焦点" << std::endl;
    //     break;
    // //键盘事件
    // case sf::Event::KeyPressed:
    //     if(event.key.code == sf::Keyboard::Escape)//判断ESC键是否被按下
    //     {
    //         std::cout << "Esc键被按下"<<std::endl;
    //     }
    //     break;
    // case sf::Event::KeyReleased:
    //     if(event.key.code == sf::Keyboard::Escape) //判断ESC键是否弹起
    //     {
    //         std::cout << "Esc键弹起"<<std::endl;
    //         window.close();
    //     }
    //     break;
    // case sf::Event::TextEntered:
    //     std::cout<<"输入字符:" << static_cast<char> (event.text.unicode) <<std::endl;
    //     break;
    // case sf::Event::MouseMoved:
    //     std::cout << "x:" << event.mouseMove.x << ",y:" << event.mouseMove.y << std::endl;
    //     break;
    // case sf::Event::MouseButtonPressed:
    //     if(event.mouseButton.button == sf::Mouse::Left)
    //     {
    //         std::cout << "MouseButtonPressed 事件, 鼠标左键被按下" <<std::endl;
    //     }
    //     else if(event.mouseButton.button== sf::Mouse::Right)
    //     {
    //         std::cout << "MouseButtonPressed 事件, 鼠标右键被按下" <<std::endl;
    //     }
    //     break;
    // case sf::Event::MouseButtonReleased:
    //         if(event.mouseButton.button == sf::Mouse::Left)
    //     {
    //         std::cout << "MouseButtonReleased 事件, 鼠标左键弹起" <<std::endl;
    //     }
    //     else if(event.mouseButton.button== sf::Mouse::Right)
    //     {
    //         std::cout << "MouseButtonReleased 事件, 鼠标右键弹起" <<std::endl;
    //     }
    //     break;
    // case sf::Event::MouseWheelScrolled:
    //     if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
    //         std::cout << "wheel type: vertical" << std::endl;
    //     else if (event.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel)
    //         std::cout << "wheel type: horizontal" << std::endl;
    //     else
    //         std::cout << "wheel type: unknown" << std::endl;
    //     std::cout << "wheel movement: " << event.mouseWheelScroll.delta << std::endl;
    //     std::cout << "mouse x: " << event.mouseWheelScroll.x << std::endl;
    //     std::cout << "mouse y: " << event.mouseWheelScroll.y << std::endl;
    //     break;
    // case sf::Event::MouseLeft:
    //         std::cout << "鼠标指针离开窗口" << std::endl;
    //     break;
    // case sf::Event::MouseEntered:
    //     std::cout << "鼠标指针进入窗口" << std::endl;
    //     break;
    // }
    //}
    //2.更新游戏对象
    //3.绘制游戏对象
    //清除屏幕
    //window.clear();
   	//绘制游戏内容
    //window.display();
    //}
    //sf::Style::None 没有边框的窗口，不能和其它样组合使用
    //sf::Style::Titlebar; 只有标题栏的窗口（没有最大化，最小化，关闭按钮）
    //sf::Style::Close 有标题栏 ，最小化，关闭按钮，最大化不能使用 的窗口,不能改变窗口大小 。
    //sf::Style::Fullscreen 全屏窗口，能和其它样式组合使用
    //sf::Style::Resize 可以改变大小的窗口
    //sf::Style::Default 有标题栏 ，最小化，最大化、关闭按钮的窗口，是Titlebar, Resize, Close 这三种样式的组合
    //setPosition (const Vector2i &position) 设置窗口位置
    //setSize (const Vector2u &size) 设置窗口大小
    //setTitle (const String &title) 设置窗口标题名称
    //setMouseCursorVisible (bool visible) 设置鼠标指针是否显示
    //setIcon (unsigned int width, unsigned int height, const Uint8 *pixels) 设置窗口图标
    //setVisible (bool visible) 设置窗口显示或隐藏
    //Vector2i getPosition () const 取得窗口的位置
    //virtual Vector2u getSize () const 取得窗口的大小
    //close () 关闭窗口
    //bool isOpen () const 判断窗口是否打开状态
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Dungeon prowler", sf::Style::None);
    Game game(&window);

    // Initialize and run the game object.
    game.Initialize();
    game.Run();

    // Exit the application.
    return 0;
}
