class SMC_t {

public:

	void swap_frameworks(); 

	const char *dlsym_error;
	void *handle;

	create_t *user_type;
	create_summary_t *user_summary_type;

	destroy_t *destroy_user_type;
	destroy_summary_t *destroy_user_summary_type;

	framework_t<param_t> **last, **current, **proposed;
	param_t **last_params;
	framework_summary_t<param_summary_t> *last_summary;


	struct timers_t { double begin,end; } timers;

	uint d,D, n,N,g,G,T, T_per_proc,A, A_per_proc, *R, cheat, sample;

	float **O,*E, last_epsilon, current_epsilon, terminal_epsilon,x,sum_weight, max_weight;
	char *output_prefix, *ptr_a,*ptr_b,*ptr_c,*line, *O_string;

	char *last_data, *current_data, *proposed_data;

	uint size_of_mem;

	void update_last_params(); 
	void sort_proposed(); 

	void calculate_weights(); 

	void print_summary();

	void print_progress(uint *_t); 

	uint set_size_of_mem ();


	void generate(); 

//enter our loop of SMC...
	void loop() {}
	
	SMC_t(rapidxml::xml_document<> *cfg) {}
	
	~SMC_t() {}

};
