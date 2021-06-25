#include<string.h>
#include<stdlib.h>

#include "schedulers.h"
#include "task.h"
#include "cpu.h"
#include "list.h"

//first come first served

struct node *taskList = NULL;

//node tiene una tarea y un puntero hacia el siguiente nodo (lista enlazada)


// add a task to the list
// agrega tarea a lista de tareas
// burst: la cantidad de tiempo que el proceso usa el procesador antes de que ya no esté listo
void add(char *name, int priority, int burst) {
   //pasan parametros para crear la tarea 
    Task *t = malloc(sizeof(Task));
    // allocate memory and then copy the name
    t->name = malloc(sizeof(char) * (strlen(name) + 1));
    //copia el name que entra y se lo pone a la tarea t
    strcpy(t->name, name);
    // priority and burst
    t->priority = priority;
    t->burst = burst;
    // insert into task list
    insert(&taskList, t);
}

/* pickNextTask: pick the next task to execute with FCFS
 * taskList must not be empty!
 */
//saca la sigueitne tarea

//                      lastnode
//null<-t0<-t1<-t2<-t3<-t4

Task *pickNextTask() {
   //ultimo nodo hace referencia a tasklist
    struct node *lastNode = taskList;
    while(1) {
        if(!lastNode->next) {//recoore esa lista 
            break;//rompe
        }
        lastNode = lastNode->next;
    }
    return lastNode->task;//para llegar a la primera tarea
}

// invoke the scheduler
void schedule() {
    while(taskList) {
        Task *t = pickNextTask();
        run(t, t->burst);
        delete(&taskList, t);
    }
}
