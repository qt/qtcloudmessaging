TEMPLATE = subdirs
SUBDIRS = cloudmessaging

embedded-kaltiot { SUBDIRS += cloudmessagingembeddedkaltiot }
firebase { SUBDIRS += cloudmessagingfirebase }

DISTFILES += \
    ../README.md
