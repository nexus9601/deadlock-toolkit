#pragma once
#include <string>

// Represents a resource type in the system
struct Resource {
    int id;
    std::string name;
    int total;      // Total instances of this resource
    int available;  // Currently available instances

    Resource() : id(-1), total(0), available(0) {}

    Resource(int id, const std::string& name, int total)
        : id(id), name(name), total(total), available(total) {}

    // Allocate 'count' units of this resource
    bool allocate(int count) {
        if (count > available) return false;
        available -= count;
        return true;
    }

    // Release 'count' units back
    void release(int count) {
        available += count;
        if (available > total) available = total;
    }
};
