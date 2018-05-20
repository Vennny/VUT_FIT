/**
 * @mainpage Project 3
 * @brief Project 3 - Basic cluster analysis - (Jednoducha shlukova analyza)
 * @author Vaclav Trampeska <xtramp00@stud.fit.vutbr.cz>
 * @date December 2017
 * @file proj3.h
 */


/**
 * @brief Structure representing an object with an ID and x and y coordinates
 */
struct obj_t {
	/** identificator */
	int id;
	/** coordinate x */
	float x;
	/** coordinate y */
	float y;
};

/**
 * @brief Structure representing a cluster of objects.
 */
struct cluster_t {
	/** number of objects in a specific cluster */
	int size;
	/** capacity of a specific cluster - number of objects we can fit in */
	int capacity;
	/** array of objects in a specific cluster */
	struct obj_t *obj;
};


/**
 * @defgroup clusterEditing Operations with clusters and allocations
 * @{
 */

/**
 * Initialisation of cluster 'c', allocates memory for all the objects that can fit in within the capacity of the cluster
 * NULL stands for 0 capacity
 *
 * @param c cluster of initialisation
 * @param cap demanded capacity of the cluster
 *
 * @post Cluster 'c' will have a memory of it's capacity allocated unless an error occurs
 */
void init_cluster(struct cluster_t *c, int cap);

/**
 * Removes all objects of the cluster and initializes the cluster to 0 capacity
 *
 * @param c cluster for clearing
 *
 * @positionAllocated memory of all the objects of the cluster will be freed
 */
void clear_cluster(struct cluster_t *c);

/// value for reallocation of a cluster
extern const int CLUSTER_CHUNK;

/**
 * Change of capacity of a cluster 'c'
 *
 * @param c cluster for capacity change
 * @param new_cap new capacity
 *
 * @pre Capacity of the cluster 'c' will be greater than or equal to 0
 *
 * @post Capacity of the cluster 'c' will be reallocated to new capacity 'new_cap' unless an error occurs
 *
 * @return cluster with a new capacity, NULL in case of any error
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap);

/**
 * Adds object 'obj' at the end of a cluster 'c1'. If there's no empty space in the cluster, extends the cluster
 *
 * @param c cluster for adding an objects
 * @param obj object which will be added at the last position of the cluster 'c'
 *
 * @pre Number of objects in the cluster is greater than or equal to 0
 *
 * @post Object 'obj' is at the end of a cluster 'c' unless an error during the allocation occurs
 */
void append_cluster(struct cluster_t *c, struct obj_t obj);

/**
 * Adds objects of cluster 'c2' to cluster 'c1'. 'c1' will be reallocated if necessary
 * Objects of 'c1' will be sorted upwardly by their ID. 'c2' stays unchanged
 *
 * @param c1 a cluster to which the objects of 'c2' will be added
 * @param c2 a cluster of which objects will be added to cluster 'c1'
 *
 * @pre Number of objects in the cluster is greater than or equal to 0
 *
 * @post Cluster 'c1' also contains objects of cluster 'c' unless an error occures 
 * Objects of 'c1' will be sorted upwardly by their ID
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);

/**
 * Sorts objects upwardly by their ID
 *
 * @param c a cluster for sorting
 *
 * @post Objects of a cluster 'c' are sorted upwardly by their ID
 */
void sort_cluster(struct cluster_t *c);

/**
 * Prints a cluster on stdout
 *
 * @param c a cluster which will be printed
 *
 * @post Objects of a cluster 'c' are printed on stdout
 */
void print_cluster(struct cluster_t *c);

/**
 * @}
 */


/**
 * @defgroup clustersArray Editing clusters array
 * @{
 */

/**
 * Removes a cluster from an array 'carr'
 *
 * @param carr array of clusters
 * @param narr nuber of clusters in the array
 * @param idx index of a cluster for removing
 *
 * @return new number of clusters in the array
 *
 * @post cluster with index 'idx' from an array of clusters 'carr' is removed. The array is shorter by 1
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx);

/**
 * Calculates Euclidean distance of two objects.
 *
 * @param o1 object 1
 * @param o2 object 2
 *
 * @return Euclidean distance of two objects 'c1'and 'c2'
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2);

/**
 * Calculates a distance of objects 'c1' and 'c2' using the unweighted pair-group average method
 *
 * @param c1 cluster 1
 * @param c2 cluster 2
 *
 * @return distance of clusters `c1` and `c2`
 *
 * @pre number of objects in cluster 'c1' will be greater than or equal to 0
 * number of objects in cluster 'c2' will be greater than or equal to 0
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);

/**
 * Finds two closest clusters. Saves indexes of of found clusters in array 'carr' 'c1' and 'c2'
 *
 * @param carr array of cluster
 * @param narr number of clusters in the array
 * @param c1 index of the first founds cluster
 * @param c2 index of the second found cluster
 *
 * @post Indexes of two closest clusters 'c1' and 'c2' are found and saved
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);

/**
 * Loads a number of objects and objects themselves from a file 'filename'. 
 * Allocates memory for an array of all clusters and saves the pointer to the first position.
 * Creates a cluster for each objects and saves them at the last position of the array of clusters.
 * 
 * @param filename name of file containg information for loading
 * @param arr pointer ton an array of clusters 
 *
 * @return number of clusters or -1 in case of an error occuring
 * 
 * @pre A file 'filename' exists 
 *
 * @post A cluster saved in array of clusters 'arr' is created for each object unless an error occures
 */
int load_clusters(char *filename, struct cluster_t **arr);

/**
 * Prints an array of cluster on stdout
 *
 * @param carr array of clusters for printing
 * @param narr number of clusters in the array
 *
 * @post All clusters and their objects in the array 'carr' are printed on stdout
 */
void print_clusters(struct cluster_t *carr, int narr);

/**
 * @}
 */