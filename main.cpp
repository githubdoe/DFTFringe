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

#ifdef _WIN32
    #include <Windows.h>
    #include <DbgHelp.h>
    #include <ctime>

    LONG WINAPI MyUnhandledExceptionFilter(EXCEPTION_POINTERS* ExceptionInfo)
    {
        // Create a timestamped filename
        time_t now = time(nullptr);
        char filename[MAX_PATH];
        strftime(filename, sizeof(filename), "DFTFringeLogs\\crashdump_%Y%m%d_%H%M%S.dmp", localtime(&now));

        HANDLE hFile = CreateFileA(filename, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile == INVALID_HANDLE_VALUE) {
            return EXCEPTION_EXECUTE_HANDLER;
        }

        if(ExceptionInfo != nullptr)
        {
            MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
            dumpInfo.ThreadId = GetCurrentThreadId();
            dumpInfo.ExceptionPointers = ExceptionInfo;
            dumpInfo.ClientPointers = FALSE;

            MiniDumpWriteDump(
                GetCurrentProcess(),
                GetCurrentProcessId(),
                hFile,
                MiniDumpNormal,
                &dumpInfo,
                nullptr,
                nullptr
            );
        }
        else
        {
            MiniDumpWriteDump(
                GetCurrentProcess(),
                GetCurrentProcessId(),
                hFile,
                MiniDumpNormal,
                nullptr, // No exception information
                nullptr,
                nullptr
            );
        }

        CloseHandle(hFile);

        return EXCEPTION_EXECUTE_HANDLER;
    }
#endif

static void my_terminate_handler() {
    try {
        spdlog::get("logger")->critical("Unexpected issue. Stacktrace:\n" + boost::stacktrace::to_string((boost::stacktrace::stacktrace())));
        MyUnhandledExceptionFilter(nullptr); // Call the unhandled exception filter to create a crash dump
    } catch (...) {}
    std::abort();
}

static void myQtMessageOutput(QtMsgType type, const QMessageLogContext &/*context*/, const QString &msg)
{
    const std::string localMsg = msg.toStdString();
    
    switch (type) {
    case QtDebugMsg:
        spdlog::get("logger")->debug("QT message handler: {}", localMsg);
        break;
    case QtInfoMsg:
        spdlog::get("logger")->info("QT message handler: {}", localMsg);
        break;
    case QtWarningMsg:
        spdlog::get("logger")->warn("QT message handler: {}", localMsg);
        break;
    case QtCriticalMsg:
    case QtFatalMsg:
        spdlog::get("logger")->critical("QT message handler: {}", localMsg);
        my_terminate_handler();
        break;
    }
}

static int myCvErrorCallback( int /*status*/, const char* /*func_name*/,
            const char* err_msg, const char* file_name,
            int line, void* /*userdata*/ )
{
    spdlog::get("logger")->critical("CV error :{} in {} on line {}", err_msg, file_name, line);
    
    my_terminate_handler();
    return 0;   //Return value is not used
}

int main(int argc, char *argv[])
{   
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

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("DFTFringeLogs/log.txt", 1048576 * 5, 3);

    auto combined_logger = std::make_shared<spdlog::logger>("logger", spdlog::sinks_init_list({console_sink, file_sink}));
    
    // Combined logger needs to be manually registered or it won't be found by "get"
    spdlog::register_logger(combined_logger);

    // periodically flush all *registered* loggers every 3 seconds:
    // warning: only use if all your loggers are thread-safe ("_mt" loggers)
    spdlog::flush_every(std::chrono::seconds(3));

#ifndef DALE_DO_NOT_LOG
    // Set the logging format
    spdlog::get("logger")->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
#endif 

    // Set logger level
    settingsDebug::setLogLevel(settingsDebug::getLogLevel());

    // those are examples
    spdlog::get("logger")->info("\r\n\r\n\r\n-------------");
    spdlog::get("logger")->trace("Welcome to spdlog version {}.{}.{}  !", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
    spdlog::get("logger")->trace("spdlog trace");
    spdlog::get("logger")->debug("spdlog debug");
    spdlog::get("logger")->info("spdlog info");
    spdlog::get("logger")->warn("spdlog warn");
    spdlog::get("logger")->error("spdlog error");
    spdlog::get("logger")->critical("spdlog critical");

    // from here, any problematic application exit (for example uncatched exceptions) should call my_terminate_handler
    std::set_terminate(&my_terminate_handler);
#ifdef _WIN32
    // in case of specific Windows exceptions, we want to catch them and write a minidump
    SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
#endif

#ifndef DALE_DO_NOT_LOG
    // override QT message handler because qFatal() and qCritical() would exit cleanly without crashlog
    qInstallMessageHandler(myQtMessageOutput); // replace with nullptr if you want to use original bahavior for debug purpose
    // override CV error handler to get crashlog to execute instead of clean exit

    cv::redirectError(myCvErrorCallback); // replace with nullptr if you want to use original bahavior for debug purpose
#endif
    spdlog::get("logger")->critical("This is a demo stacktrace:\n" + boost::stacktrace::to_string((boost::stacktrace::stacktrace())));

    MainWindow *w = new MainWindow;
    msgReceiver.m_mainWindow = w;

#if defined(_WIN32)
    //showmem();
#endif
    w->show();
    //fclose(logptr);
    return app.exec();
}
