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
    while True:
        logger.set_log_level(logger.DEBUG)
        logger.log(logger.ERROR, 1, 2, 3, sep="_")
        logger.log(logger.WARN, locals())
        logger.info("hey", "heya", 1, 2, 3)
        logger.debug("hey", "heya", 1, 2, 3, sep = ", ")
        render.draw_quad()