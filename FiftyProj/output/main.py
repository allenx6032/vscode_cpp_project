from pype import *

def load_fox_anim(name):
    anim = FrameAnimation()
    anim.frames = load_dir(f"assets/fox/{name}")
    anim.speed = 12
    anim.loop = True
    return anim

class Player(Sprite2D):
    def on_ready(self) -> None:
        self.scale = 0.5
        self.position = Vector2(400, 300)

        self.animator = FrameAnimator()
        self.animator['run'] = load_fox_anim('run')
        self.animator['walk'] = load_fox_anim('walk')
        self.animator.parent = self
        self.animator.play('walk')

    def on_update(self) -> None:
        delta_x = 0
        if Input.get_key(KeyCode.A):
            delta_x -= 1
        if Input.get_key(KeyCode.D):
            delta_x += 1
        if delta_x != 0:
            self.animator.play('run')
            self.flip = delta_x < 0
        else:
            self.animator.play('walk')
        self.x += delta_x * Time.delta_time * 200

Player()
