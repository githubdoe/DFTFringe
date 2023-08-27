/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
#include "mainwindow.h"
#include <QApplication>
#include "singleapplication.h"
#include "messagereceiver.h"
#include "utils.h"
#include <QDebug>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "boost/stacktrace.hpp"

void my_terminate_handler() {
    try {
        spdlog::get("logger")->critical("Unexpected issue. Stacktrace:");
        spdlog::get("logger")->critical(boost::stacktrace::to_string((boost::stacktrace::stacktrace())));
    } catch (...) {}
    std::abort();
}

int main(int argc, char *argv[])
{   
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("DFTFringeLogs/log.txt", 1048576 * 5, 3);

    auto combined_logger = std::make_shared<spdlog::logger>("logger", spdlog::sinks_init_list({console_sink, file_sink}));
    
    // Combined logger needs to be manually registered or it won't be found by "get"
    spdlog::register_logger(combined_logger);

    // periodically flush all *registered* loggers every 3 seconds:
    // warning: only use if all your loggers are thread-safe ("_mt" loggers)
    spdlog::flush_every(std::chrono::seconds(3));

    // Set the logging format
    spdlog::get("logger")->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

    // those are examples
    spdlog::trace("Welcome to spdlog version {}.{}.{}  !", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
    spdlog::get("logger")->trace("spdlog trace");
    spdlog::get("logger")->debug("spdlog debug");
    spdlog::get("logger")->info("spdlog info");
    spdlog::get("logger")->warn("spdlog warn");
    spdlog::get("logger")->error("spdlog error");
    spdlog::get("logger")->critical("spdlog critical");

    // from here, any problematic application exit should call my_terminate_handler
    std::set_terminate(&my_terminate_handler);

    spdlog::get("logger")->critical("This is a demo stacktrace");
    spdlog::get("logger")->critical(boost::stacktrace::to_string((boost::stacktrace::stacktrace())));

    // Allow secondary instances
    SingleApplication app( argc, argv, true );

    MessageReceiver msgReceiver;

    // If this is a secondary instance
    if( app.isSecondary() ) {
        app.sendMessage( app.arguments().join('\'').toUtf8() );
        return 0;
    } else {
        QObject::connect(
            &app,
            &SingleApplication::receivedMessage,
            &msgReceiver,
            &MessageReceiver::receivedMessage
        );
    }



    app.setOrganizationName("DFTFringe");
    app.setApplicationName("DFTFringe");
    MainWindow *w = new MainWindow;
    msgReceiver.m_mainWindow = w;

#if defined(_WIN32)
    //showmem();
#endif
    w->show();
    //fclose(logptr);
    return app.exec();
}
