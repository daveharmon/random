/*
 *
 */

#include <stdlib.h>
#include <mpi.h>

/*
 * Default ordering function for the sort function
 */
int IncOrder(const void *e1, const void *e2);

/*
 * 
 */
int CompareSplit(int n_local, int *elems, int *relems,
        int *w_space, int keep_small);

int main(int argc, char *argv[])
{
    int n;          /* the total number of elements to be sorted */
    int np;         /* the total number of processes */
    int m_rank;     /* the rank of the calling process */
    int n_local;    /* the number of local elements */
    int *elems;     /* the array of local elements */
    int *relems;    /* the buffer of received elements */
    int odd_rank;   /* the rank of the process during odd-phase comms */
    int even_rank;  /* the rank of the process during even-phase comms */
    int *w_space;   /* scratch space during the compare-split op */
    int i;
    MPI_Status status;

    /* Initialize MPI and get system information for bookkeeping */
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &m_rank);

    n = atoi(argv[1]);
    n_local = n / np;   /* compute num elems to be sorted locally */

    /* allocate memory for instance arrays */
    elems = (int *) malloc(n_local * sizeof(int));
    relems = (int *) malloc(n_local * sizeof(int));
    w_space = (int *) malloc(n_local * sizeof(int));

    /* fill in our elements array with random elements */
    srandom(m_rank);
    for (i = 0; i < n_local; i++) {
        elems[i] = random();
    }

    /* sort local elements using built-in quicksort routine */
    qsort(elems, n_local, sizeof(int), IncOrder);

    /* determine the processes that this process communicates with during
     * the odd and even phases */
    if (m_rank % 2 == 0) {
        odd_rank = m_rank - 1;
        even_rank = m_rank + 1;
    } else {
        odd_rank = m_rank + 1;
        even_rank = m_rank - 1;
    }

    /* set the ranks of processes at each end of linear spectrum */
    if (odd_rank == -1 || odd_rank == np) {
        odd_rank = MPI_PROC_NULL;
    }
    if (even_rank == -1 || even_rank == np) {
        even_rank = MPI_PROC_NULL;
    }

    /* main loop of the algorithm */
    for (i = 0; i < np - 1; i++) {
        if (i % 2 == 1) {   /* odd phase */
            MPI_Sendrecv(elems, n_local, MPI_INT, odd_rank, 1, relems,
                    n_local, MPI_INT, odd_rank, 1, MPI_COMM_WORLD, &status);
        } else {    /* even phase */
            MPI_Sendrecv(elems, n_local, MPI_INT, even_rank, 1, relems,
                    n_local, MPI_INT, even_rank, 1, MPI_COMM_WORLD, &status);
        }

        CompareSplit(n_local, elems, relems, w_space,
                m_rank < status.MPI_SOURCE);
    }

    free(elems);
    free(relems);
    free(w_space);
    MPI_Finalize();
}

int CompareSplit(int n_local, int *elems, int *relems,
        int *w_space, int keep_small)
{
    int i, j, k;

    for (i = 0; i < n_local; i++) {
        w_space[i] = elems[i];
    }

    if (keep_small) {   /* keep the n_local smaller elems */
        for (i = j = k = 0; k < n_local; k++) {
            if (j == n_local || (i < n_local && w_space[i] < relems[j])) {
                elems[k] = w_space[i++];
            } else {
                elems[k] = relems[j++];
            }
        }
    } else {    /* keep the n_local larger elems */
        for (i = j = k = n_local - 1; k >= 0; k--) {
            if (j == 0 || (i >= 0 && w_space[i] >= relems[j])) {
                elems[k] = w_space[i--];
            } else {
                elems[k] = relems[j--];
            }
        }
    }

    return 0;
}

/*
 *
 */
int IncOrder(const void *e1, const void *e2)
{
    return (*((int *)e1) - *((int *)e2));
}
