function __FILE__() return debug.getinfo(4,'S').source end
function __LINE__() return debug.getinfo(4, 'l').currentline end

log = {}
log.LOG_TRACE = 0
log.LOG_DEBUG = 1
log.LOG_INFO = 2
log.LOG_WARN = 3
log.LOG_ERROR = 4
log.LOG_FATAL = 5
log._log = function(level, fmt, ...)
    log_log(level, __FILE__(), __LINE__(), string.format(fmt, ...))
end
log.trace = function(fmt, ...)
    log._log(log.LOG_TRACE, fmt, ...)
end
log.debug = function(fmt, ...)
    log._log(log.LOG_DEBUG, fmt, ...)
end
log.info = function(fmt, ...)
    log._log(log.LOG_INFO, fmt, ...)
end
log.warn = function(fmt, ...)
    log._log(log.LOG_WARN, fmt, ...)
end
log.error = function(fmt, ...)
    log._log(log.LOG_ERROR, fmt, ...)
end
log.fatal = function(fmt, ...)
    log._log(log.LOG_FATAL, fmt, ...)
end
