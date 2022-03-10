#include "multi_timer.h"

#include <unistd.h>     // sleep

void run_timer(mtimer_t t)
{
    // TODO:
    // - Display countdown
    // - Fork this process?
    //   Do we care if this blocks the application?
    sleep(t.duration);
    if (t.on_complete) t.on_complete(t.data);
}
