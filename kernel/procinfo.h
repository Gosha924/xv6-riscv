#ifndef KERNEL_PROCINFO_H
#define KERNEL_PROCINFO_H


struct procinfo {
    int id;        
    char name[16];
    int state;   
    int parent_id;     
};

typedef struct procinfo procinfo_type;

#endif