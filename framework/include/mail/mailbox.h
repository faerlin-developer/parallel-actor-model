#ifndef MAILBOX_H
#define MAILBOX_H

#include <queue>
#include <unordered_map>
#include "mail/types.h"
#include "mail/message.h"
#include "actor/types.h"

namespace mail {

    /**
     * Actors require access to certain data owned by the framework.
     * This structure contains pointers to that data and is provided to each actor.
     */
    struct Context {
        std::vector<mail::Type> *mail_types;
        std::unordered_map<actor::id, mail::Address> *id_to_address;
    };

    /**
     * Each actor is assigned a Mailbox.
     * Through the Mailbox API, an actor is able to send and receive messages.
     */
    class Mailbox {
    public:
        Address address{};  // An address uniquely identifying this mailbox
        Context context{};  // Holds pointers to data owned by the framework

    public:

        Mailbox();

        Mailbox(Address address, Context context);

        bool hasMessage() const;

        Message receive() const;

        void send(Message &msg, actor::id to) const;

        ~Mailbox();
    };
}

#endif
