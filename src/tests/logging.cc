#include "../util/logging.h"

int main(int argc, char const *argv[])
{
    logging::Logger logger;
    logger.set_log_level(logging::LogLevel::DEBUG);
    logger.debug("tötet die käfer");
    logger.info("INFOrmatik");
    logger.warn("FutureWarning");
    logger.error("you failed your life");
    return 0;
}
