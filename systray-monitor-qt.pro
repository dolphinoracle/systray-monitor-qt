#-------------------------------------------------
#
# Project created by QtCreator 2016-04-19T14:58:51
#
#-------------------------------------------------

#DEFINES += QT_NO_WARNING_OUTPUT QT_NO_DEBUG_OUTPUT

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = systray-monitor-qt
TEMPLATE = app


SOURCES += main.cpp \
    about.cpp \
    mainwindow.cpp

HEADERS  += \
    about.h \
    mainwindow.h

FORMS    += \
    mainwindow.ui

TRANSLATIONS += translations/systray-monitor-qt_am.ts \
                translations/systray-monitor-qt_ar.ts \
                translations/systray-monitor-qt_bg.ts \
                translations/systray-monitor-qt_ca.ts \
                translations/systray-monitor-qt_cs.ts \
                translations/systray-monitor-qt_da.ts \
                translations/systray-monitor-qt_de.ts \
                translations/systray-monitor-qt_el.ts \
                translations/systray-monitor-qt_en.ts \
                translations/systray-monitor-qt_es.ts \
                translations/systray-monitor-qt_et.ts \
                translations/systray-monitor-qt_eu.ts \
                translations/systray-monitor-qt_fa.ts \
                translations/systray-monitor-qt_fi.ts \
                translations/systray-monitor-qt_fr.ts \
                translations/systray-monitor-qt_fr_BE.ts \
                translations/systray-monitor-qt_he_IL.ts \
                translations/systray-monitor-qt_hi.ts \
                translations/systray-monitor-qt_hr.ts \
                translations/systray-monitor-qt_hu.ts \
                translations/systray-monitor-qt_id.ts \
                translations/systray-monitor-qt_is.ts \
                translations/systray-monitor-qt_it.ts \
                translations/systray-monitor-qt_ja.ts \
                translations/systray-monitor-qt_kk.ts \
                translations/systray-monitor-qt_ko.ts \
                translations/systray-monitor-qt_lt.ts \
                translations/systray-monitor-qt_mk.ts \
                translations/systray-monitor-qt_mr.ts \
                translations/systray-monitor-qt_nb.ts \
                translations/systray-monitor-qt_nl.ts \
                translations/systray-monitor-qt_pl.ts \
                translations/systray-monitor-qt_pt.ts \
                translations/systray-monitor-qt_pt_BR.ts \
                translations/systray-monitor-qt_ro.ts \
                translations/systray-monitor-qt_ru.ts \
                translations/systray-monitor-qt_sk.ts \
                translations/systray-monitor-qt_sl.ts \
                translations/systray-monitor-qt_sq.ts \
                translations/systray-monitor-qt_sr.ts \
                translations/systray-monitor-qt_sv.ts \
                translations/systray-monitor-qt_tr.ts \
                translations/systray-monitor-qt_uk.ts \
                translations/systray-monitor-qt_zh_CN.ts \
                translations/systray-monitor-qt_zh_TW.ts

CONFIG += release warn_on thread qt c++11
