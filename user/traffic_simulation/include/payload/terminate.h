#ifndef TERMINATE_H
#define TERMINATE_H

namespace payload {

    /**
     * Terminate message sent by summary actor to all other actors (corresponds to MPI_TERMINATE).
     */
    struct Terminate {
        int dummy;
    };
}


#endif
