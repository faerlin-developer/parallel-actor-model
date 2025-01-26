#ifndef ACTORS_H
#define ACTORS_H

#include <unistd.h>
#include "actor/actor.h"

class HeadNode : public actor::Actor {
public:
    int value;

    HeadNode(int id, int value);

    actor::next_step run() override;
};

class Node : public actor::Actor {
public:
    int value;
    bool has_sent_initial_value;

    Node(int id, int value);

    actor::next_step run() override;
};

class TailNode : public actor::Actor {
public:
    int value;

    TailNode(int id, int value);

    actor::next_step run() override;
};

#endif
