"""pype核心模块"""

from typing import Any, Callable, Generator, Iterable, Tuple, List, overload
from abc import abstractmethod
import easing

# 根节点
_root: 'Node' = ...

class Color:
    def __init__(self, r: int, g: int, b: int, a: int=255): ...
    def __add__(self, rhs: 'Color') -> 'Color': ...
    def __sub__(self, rhs: 'Color') -> 'Color': ...
    def __mul__(self, rhs: float) -> 'Color': ...
    def __truediv__(self, rhs: float) -> 'Color': ...
    def __eq__(self, rhs: 'Color') -> bool: ...
    def __ne__(self, rhs: 'Color') -> bool: ...

class Colors:
    White: 'Color' = ...
    Black: 'Color' = ...
    Red: 'Color' = ...
    Green: 'Color' = ...
    Blue: 'Color' = ...
    Yellow: 'Color' = ...
    Cyan: 'Color' = ...
    Magenta: 'Color' = ...
    Transparent: 'Color' = ...

class Vector2:
    x: float
    y: float
    def __init__(self, x: float, y: float): ...
    def __add__(self, rhs: 'Vector2') -> 'Vector2': ...
    def __sub__(self, rhs: 'Vector2') -> 'Vector2': ...
    def __mul__(self, rhs: float) -> 'Vector2': ...
    def __truediv__(self, rhs: float) -> 'Vector2': ...
    def __eq__(self, rhs: 'Vector2') -> bool: ...
    def __ne__(self, rhs: 'Vector2') -> bool: ...

class Rect:
    x: float
    y: float
    w: float
    h: float
    @overload
    def __init__() -> None: ...
    @overload
    def __init__(self, pos: Vector2, size: Vector2): ...
    @overload
    def __init__(self, x: float, y: float, w: float, h: float): ...
    def center(self) -> Vector2: ...
    def size(self) -> Vector2: ...

class Texture2D:
    @property
    def width(self) -> int: ...
    @property
    def height(self) -> int: ...
    @property
    def limit_left(self) -> int: ...
    @property
    def limit_right(self) -> int: ...
    @property
    def limit_top(self) -> int: ...
    @property
    def limit_bottom(self) -> int: ...

class Font:
    pass

class gl:
    @staticmethod
    def draw_texture(tex: Texture2D, pos: Vector2, angle: float, scale: float, color: Color, flip: bool) -> None:
        """绘制纹理"""
    @staticmethod
    def draw_line(x0, y0, x1, y1, line_width, color: Color) -> None:
        """绘制线段"""
    @staticmethod
    def draw_rect(rect: Rect, line_width, color: Color, border_radius: float) -> None:
        """绘制空心矩形"""
    @staticmethod
    def draw_solid_rect(rect: Rect, color: Color, border_radius: float) -> None:
        """绘制实心矩形"""
    @staticmethod
    def draw_circle(x, y, radius: float, line_width, color: Color) -> None:
        """绘制空心圆"""
    @staticmethod
    def draw_solid_circle(x, y, radius: float, color: Color) -> None:
        """绘制实心圆"""

class Signal:
    def connect(self, f: Callable) -> None:
        """连接一个信号处理函数"""
    def disconnect(self, f: Callable) -> None:
        """断开一个信号处理函数"""
    def emit(self, *args) -> None:
        """发射信号"""
    def disconnect_all(self) -> None:
        """断开所有信号处理函数"""

class Time:
    delta_time: float
    time: float

class KeyCode:
    Undefined: int = 0
    Alpha0: int = 1
    Alpha1: int = 2
    Alpha2: int = 3
    Alpha3: int = 4
    Alpha4: int = 5
    Alpha5: int = 6
    Alpha6: int = 7
    Alpha7: int = 8
    Alpha8: int = 9
    Alpha9: int = 10
    A: int = 11
    B: int = 12
    C: int = 13
    D: int = 14
    E: int = 15
    F: int = 16
    G: int = 17
    H: int = 18
    I: int = 19
    J: int = 20
    K: int = 21
    L: int = 22
    M: int = 23
    N: int = 24
    O: int = 25
    P: int = 26
    Q: int = 27
    R: int = 28
    S: int = 29
    T: int = 30
    U: int = 31
    V: int = 32
    W: int = 33
    X: int = 34
    Y: int = 35
    Z: int = 36
    F1: int = 37
    F2: int = 38
    F3: int = 39
    F4: int = 40
    F5: int = 41
    F6: int = 42
    F7: int = 43
    F8: int = 44
    F9: int = 45
    F10: int = 46
    F11: int = 47
    F12: int = 48
    F13: int = 49
    F14: int = 50
    F15: int = 51
    F16: int = 52
    F17: int = 53
    F18: int = 54
    F19: int = 55
    F20: int = 56
    F21: int = 57
    F22: int = 58
    F23: int = 59
    F24: int = 60
    Keypad0: int = 61
    Keypad1: int = 62
    Keypad2: int = 63
    Keypad3: int = 64
    Keypad4: int = 65
    Keypad5: int = 66
    Keypad6: int = 67
    Keypad7: int = 68
    Keypad8: int = 69
    Keypad9: int = 70
    KeypadClear: int = 71
    KeypadDecimal: int = 72
    KeypadDivide: int = 73
    KeypadEnter: int = 74
    KeypadEquals: int = 75
    KeypadMinus: int = 76
    KeypadMultiply: int = 77
    KeypadPlus: int = 78
    Backslash: int = 79
    Comma: int = 80
    Equals: int = 81
    Grave: int = 82
    LeftBracket: int = 83
    Minus: int = 84
    Period: int = 85
    Quote: int = 86
    RightBracket: int = 87
    Semicolon: int = 88
    Slash: int = 89
    Meta: int = 90
    Ctrl: int = 91
    Function: int = 92
    Alt: int = 93
    Shift: int = 94
    RightMeta: int = 95
    RightCtrl: int = 96
    RightAlt: int = 97
    RightShift: int = 98
    CapsLock: int = 99
    NumLock: int = 100
    ScrollLock: int = 101
    DownArrow: int = 102
    LeftArrow: int = 103
    RightArrow: int = 104
    UpArrow: int = 105
    End: int = 106
    Home: int = 107
    PageDown: int = 108
    PageUp: int = 109
    ContextMenu: int = 110
    Backspace: int = 111
    Escape: int = 112
    Delete: int = 113
    Help: int = 114
    Mute: int = 115
    PrintScreen: int = 116
    Enter: int = 117
    Space: int = 118
    Tab: int = 119
    VolumeDown: int = 120
    VolumeUp: int = 121
    Pause: int = 122
    Insert: int = 123
    Sleep: int = 124

    @staticmethod
    def nameof(c: int) -> str:
        """获取键码的字符串名称"""

class Input:
    @staticmethod
    def get_key(c: int) -> bool:
        """检查当前帧某个按键是否处于按下状态"""
    @staticmethod
    def get_key_down(c: int) -> bool:
        """检查某个按键是否刚刚被按下"""
    @staticmethod
    def get_key_up(c: int) -> bool:
        """检查某个按键是否刚刚被释放"""
    @staticmethod
    def get_mouse_button_down(c: int) -> bool:
        """检查某个鼠标按键是否刚刚被按下"""
    @staticmethod
    def get_mouse_button_up(c: int) -> bool:
        """检查某个鼠标按键是否刚刚被释放"""

#####################################################

class Node:
    name: str           # 节点的名称
    parent: 'Node'      # 节点的父节点
    position: Vector2   # 节点的本地坐标
    x: float            # 节点的本地x坐标
    y: float            # 节点的本地y坐标
    angle: float        # 节点的本地角度
    scale: float        # 节点的本地缩放
    enabled: bool       # 节点是否激活，激活的节点会触发`on_update`事件
    visible: bool       # 节点是否可见，可见的节点会触发渲染事件
    width: int          # 节点的宽度
    height: int         # 节点的高度

    @property
    def global_position(self) -> Vector2:
        """获取全局坐标"""
    @property
    def global_angle(self) -> float:
        """获取全局角度"""
    @property
    def global_scale(self) -> float:
        """获取全局缩放"""
    @property
    def global_transform(self) -> Tuple[Vector2, float, float]:
        """获取全局变换，返回`(position, angle, scale)`"""

    @property
    def child_count(self) -> int:
        """获取子节点的数量"""
    @property
    def children(self) -> Tuple['Node']:
        """返回一个包含所有子节点的元组"""

    @abstractmethod
    def on_ready(self) -> None:
        """[消息] 当节点被创建时调用"""
    @abstractmethod
    def on_update(self) -> None:
        """[消息] 每帧调用一次"""
    @abstractmethod
    def on_destroy(self) -> None:
        """[消息] 当节点被销毁时调用"""
    @abstractmethod
    def _draw(self) -> None:
        """[消息] 绘制节点（仅`Node`的直接子类会调用）"""

    def to(self, name: str, target: Any, duration: float, ease=None) -> 'Tweener':
        """将`self.name`在`duration`秒内变化到`target`，变化方式由`ease`指定"""

    def start_coroutine(self, obj) -> Generator:
        """启动一个协程"""
    def stop_coroutine(self, obj) -> None:
        """停止一个协程"""
    def stop_all_coroutines(self) -> None:
        """停止所有协程"""

class Sprite2D(Node):
    texture: Texture2D   # 精灵的纹理
    color: Color         # 精灵的颜色
    flip: bool           # 精灵是否翻转

class Label(Node):
    text: str
    font: Font

class TextEdit(Node):
    text: str
    font: Font
    color: Color
    background: Color
    border_color: Color
    cursor_color: Color     # 有BUG，没有作用

    def set_max_length(self, length: int) -> None:
        """设置最大长度"""
    def set_focus(self, value: bool) -> None:
        """设置是否获取焦点"""
    def set_border(self, value: bool) -> None:
        """设置是否显示边框"""
    def set_auto_newline(self, value: bool, spacing: int) -> None:
        """设置是否自动换行，参见`PX_Object_EditAutoNewLine`"""

class ProgressBar(Node):
    color: Color
    value: float    # 进度条的值，取值范围为[0, 1]

class Button(Node):
    text: str
    clicked: Signal

class Image(Node):
    texture: Texture2D

    def set_native_size(self) -> None:
        """将节点的宽高设置为纹理的宽高"""

#####################################################

class Tween:
    Ready = 0
    Playing = 1
    Completed = 2

    completed: Signal   # 完成时触发此信号

    @property
    def state(self) -> int:
        """获取当前状态"""
    def is_ready(self) -> bool:
        """检查当前状态是否为`Ready`"""
    def is_playing(self) -> bool:
        """检查当前状态是否为`Playing`"""
    def is_completed(self) -> bool:
        """检查当前状态是否为`Completed`"""

    def play(self) -> None:
        """开始播放"""

class Tweener(Tween):
    ease = easing.EaseOutQuad
    def __init__(self, obj, name, target, duration):
        self.obj = obj                  # 目标对象
        self.name = name                # 目标属性
        self.target = target            # 最终值
        self.duration = duration        # 持续时间

class TweenList(Tween):
    def append(self, tween: Tween):
        """添加一个Tween实例"""

#####################################################
def set_camera_position(pos: Vector2) -> None:
    """设置摄像机的位置"""

def WaitForEndOfFrame() -> Generator:
    """[协程] 等待当前帧结束"""
def WaitForSeconds(seconds: float) -> Generator:
    """[协程] 等待`seconds`秒"""
def WaitForSignal(signal: Signal) -> Generator:
    """[协程] 等待信号触发（实验性）"""
def WaitUntil(condition: Callable) -> Generator:
    """[协程] 等待`condition`返回`True`"""

def load(path: str) -> Any:
    """加载一个资源。此函数带有缓存，多次调用同一个路径不会重复加载"""
def load_dir(path: str) -> List:
    """加载一个目录下的所有资源，返回一个列表"""

def traverse() -> Generator:
    """返回一个可以遍历对象树所有物体的迭代器，每个项`(Node, int)`表示物体及其深度（从0开始）"""
def destroy(obj: Node) -> None:
    """销毁一个节点"""
def memory_usage() -> None:
    """打印当前的内存使用信息"""
def _print_tree() -> None:
    """用于调试，向控制台打印对象树结构"""
def _repl() -> None:
    """打开一个简易REPL，`eval`用户输入的代码，并输出结果。可以使用`exit()`退出"""

#####################################################

class FrameAnimation:
    """逐帧动画结构体"""
    frames: List[Texture2D] # 帧列表
    speed: int              # 每秒播放的帧数
    loop: bool              # 是否循环播放

class FrameAnimator(Node):
    """逐帧动画控制器，通过控制`parent`节点的`texture`属性播放序列帧动画"""
    def __setitem__(self, name: str, anim: FrameAnimation):
        """增加一个帧动画"""
    def __getitem__(self, name: str) -> FrameAnimation:
        """获取一个帧动画"""
    def play(self, name: str) -> None:
        """播放一个帧动画"""
    def stop(self) -> None:
        """停止播放"""

#####################################################