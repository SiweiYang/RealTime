#ifdef NOTIFIER_H
#else
#define NOTIFIER_H

#define CONFIRM "CONF...."

#define NOTIFIER_START      0
#define NOTIFIER_RETURN     1

#define NOTIFIER_GET        0
#define NOTIFIER_PUT        1
void cts_notifier();

void notifier();

void notifier_io();
#endif
