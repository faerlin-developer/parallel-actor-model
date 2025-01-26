#ifndef ACTOR_H
#define ACTOR_H

#include "mail/mailbox.h"
#include "mail/types.h"
#include "mail/message.h"
#include "actor/types.h"

namespace actor {

    enum next_step {
        CONTINUE,
        STOP,
    };

    /**
     * An actor capable of performing tasks, including sending messages to other actors.
     */
    class Actor {

    public:
        actor::id id;            // Uniquely identifies an actor
        mail::Mailbox mailbox;   // Allows actor to send and receive messages

    public:

        Actor() = delete;

        explicit Actor(int id);

        virtual bool pre_barrier_init();

        virtual bool post_barrier_init();

        virtual next_step ingress(mail::Message &message);

        virtual next_step run() = 0;

        virtual ~Actor();

        void finalize();
    };
}

#endif
