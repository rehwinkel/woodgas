class MyPyComponent(Component):
    def init():
        print("initing mypy")

    def update():
        print("updaten")

class MySecondPyComponent(Component):
    def init():
        print("initing second mypy")

    def update():
        print("updaten")

def main():
    ar = 640.0 / 480.0
    render.upload_orthographic(-1 * ar, 1 * ar, -1, 1, 0.1, 100)
    while render.is_window_open():
        render.poll_inputs()
        render.clear()
        render.upload_transform(0.5, 0.3, 0, 0, 0, 0.3, 1, 1, 1)
        render.draw_quad()
        render.swap_buffers()