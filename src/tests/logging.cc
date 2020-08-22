#include "../util/logging.h"

int main(int argc, char const *argv[]) {
    logging::Logger logger;
    logger.set_log_level(logging::LogLevel::DEBUG);
    logger.debug("tötet die käfer");
    logger.info("INFOrmatik");
    logger.warn("FutureWarning");
    logger.error("you failed your life");
    logger.set_log_level(logging::LogLevel::INFO);
    logger.debug("tötet die käfer");
    logger.info("INFOrmatik");
    logger.warn("FutureWarning");
    logger.error("you failed your life");
    logger.set_log_level(logging::LogLevel::WARN);
    logger.debug("tötet die käfer");
    logger.info("INFOrmatik");
    logger.warn("FutureWarning");
    logger.error("you failed your life");
    logger.set_log_level(logging::LogLevel::ERROR);
    logger.debug("tötet die käfer");
    logger.info("INFOrmatik");
    logger.warn("FutureWarning");
    logger.error("you failed your life");

    logger.set_log_level(logging::LogLevel::DEBUG);
    logger.debug_stream() << "tötet die käfer" << std::endl;
    logger.info_stream() << "INFOrmatik" << std::endl;
    logger.warn_stream() << "FutureWarning" << std::endl;
    logger.error_stream() << "you failed your life" << std::endl;
    logger.set_log_level(logging::LogLevel::INFO);
    logger.debug_stream() << "tötet die käfer" << std::endl;
    logger.info_stream() << "INFOrmatik" << std::endl;
    logger.warn_stream() << "FutureWarning" << std::endl;
    logger.error_stream() << "you failed your life" << std::endl;
    logger.set_log_level(logging::LogLevel::WARN);
    logger.debug_stream() << "tötet die käfer" << std::endl;
    logger.info_stream() << "INFOrmatik" << std::endl;
    logger.warn_stream() << "FutureWarning" << std::endl;
    logger.error_stream() << "you failed your life" << std::endl;
    logger.set_log_level(logging::LogLevel::ERROR);
    logger.debug_stream() << "tötet die käfer" << std::endl;
    logger.info_stream() << "INFOrmatik" << std::endl;
    logger.warn_stream() << "FutureWarning" << std::endl;
    logger.error_stream() << "you failed your life" << std::endl;
    return 0;
}
