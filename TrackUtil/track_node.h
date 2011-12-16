#ifdef TRACK_NODE_H
#else
#define TRACK_NODE_H

#ifndef TEST
#define NULL          0
#endif

#define NODE_NONE     0
#define NODE_SENSOR   1
#define NODE_BRANCH   2
#define NODE_MERGE    3
#define NODE_ENTER    4
#define NODE_EXIT     5

#define DIR_AHEAD     0
#define DIR_STRAIGHT  0
#define DIR_CURVED    1
#define DIR_IGNORE    0xFFFF

struct track_node;
typedef struct track_node track_node;
typedef struct track_edge track_edge;

struct track_edge {
  track_edge *reverse;
  track_node *src, *dest;
  int dist;             /* in millimetres */
  int fraction;
};

struct track_node {
  const char *name;
  int type;
  int num;              /* sensor or switch number */
  track_node *reverse;  /* same location, but opposite direction */
  track_edge edge[2];
};

#endif
