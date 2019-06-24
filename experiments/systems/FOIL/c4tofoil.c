/*****************************************************************************/
/*                                                                           */
/* Program to convert files from the standard C4.5 input format to a form    */
/* that can be used by FOIL                                                  */
/*                                                                           */
/* The relation to be found by FOIL will be of the form -                    */
/* is first class named in the .names file                                   */
/*                                                                           */
/* Hence changing the order of the class names will cause FOIL to find other */
/* relations from the same data                                              */
/*                                                                           */
/* Compilation and use:                                                      */
/* cc -o cf c4tofoil.c (produce executable cf)                               */
/* cf -f filestem (take filestem.names and filestem.data (and filestem.test  */
/*                 if present) and produce filestem.d for FOIL)              */
/* option -v produces some extra output on the standard output               */
/*                                                                           */
/* (Any error messages are currently printed on the standard output stream)  */
/*                                                                           */
/* Modification required to filestem.names:                                  */
/* Each line containing attribute information should have information        */
/* specifying the type - this is added as a C4.5 comment thus...             */
/*                                                                           */
/* (attribute info for C4.5) | type: typename                                */
/*                                                                           */
/* where typename is the name of the type of this attribute. Each typename   */
/* shall start with a letter (upper or lower case) and no typename shall be  */
/* the prefix of another typename.                                           */
/* (The latter restriction is required as the output for FOIL distinguishes  */
/* between constants of different types by prefixing them with their         */
/* typename).                                                                */
/*                                                                           */
/* For example:                                                              */
/* aardvarkish: true, false. | type: Boolean                                 */
/*                                                                           */
/* Note that values of discrete attributes which occur in the data file      */
/* become theory constants for FOIL. (However those that occur in the test   */
/* file but not data file, are just constants, not theory constants).        */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXNAMELENGTH 10000 /* Vast possible name length */

#define SkipComment     while ( ( (c = getc(fp)) != '\n' ) && (c!=EOF) )

typedef struct _att_info *att_info; /* Attribute information */
struct _att_info
{
    char *name;
    int ignore; /* !0 if to be ignored */
    int discrete; /* !0 if discrete */
    int discrete_n; /* the max number of values if "discrete N", else 0 */
    int n_values; /* values stored for discrete attributes */
    char **values;
    int *value_occurs_tr; /* number of occurrences in training of value */
    char *type_name;
    int type_number; /* number of the corresponding type */
};

typedef struct _type_info *type_info; /* Type information */
struct _type_info
{
    char *name;
    int n_atts; /* number of attributes of this type */
    int *atts; /* attributes of this type */
    int discrete; /* !0 if discrete */
    int n_values; /* values stored for discrete types */
    char **values; 
    int *value_occurs_tr; /* number of occurrences in training of value */
};   

int terminator; /* Character(/EOF) which caused previous scan to end */
char *name; /* Current name being processed */

char *copy_string(char *s);
int  get_next_name(FILE *fp, int skip, int embed);
void get_type_name(att_info attribute, FILE *fp);

/*****************************************************************************/
/* main - monolithic program to read the C4.5-style files and produce the    */
/*        FOIL-style one.                                                    */
/*****************************************************************************/

main(int argc, char *argv[])
{
    extern char *optarg;
    extern int optind;
    int o;

    int verbosity=0;
    char *filestem=NULL, *filename;

    FILE *fp, *tp;

    char **class_names=NULL;
    int n_class_names;

    char ***cases;
    int n_cases;

    char ***testing_cases;
    int n_testing_cases;

    att_info *attributes, attribute;
    int n_attributes;

    type_info *types, type;
    int n_types;

    int i, j, k;
    int c;

    /* Initialise the name char array */
    name = (char *) malloc(MAXNAMELENGTH*sizeof(char));


    /* Option handling */

    while ( (o=getopt(argc,argv,"vf:")) != -1 )
    {
        switch (o)
	{
	  case 'f':   filestem = copy_string(optarg);
                        break;
	  case 'v':   verbosity++;
	                break;
	  case '?':   printf("Option error in c4 to FOIL conversion\n");
                        exit(1);
	}
    }

    if(!filestem)
    {
        printf("Must enter filestem: -f filestem\n");
	exit(1);
    }

    /* Extract the class and attribute/type information from the .names file */

    filename = (char*) malloc((strlen(filestem)+7)*sizeof(char));
    sprintf(filename,"%s.names",filestem);
    fp = fopen(filename,"r");

    if(!fp)
    {
        printf("Can't open %s\n", filename);
	exit(1);
    }

    if(verbosity) printf("Reading from %s\n", filename);
    
    /* Get class names */

    n_class_names = 0;
    class_names = (char**) malloc(11*sizeof(char*));

    do
    {
        if(!get_next_name(fp,1,1))
	{
	    printf("Problem while reading class names\n");
	    exit(1);
	}
        if(n_class_names&&!(n_class_names%10)) 
	    class_names = (char**) realloc((void*)class_names,
					   (n_class_names+11)*sizeof(char*));
	class_names[n_class_names++] = copy_string(name);
    }while(terminator==',');	  


    if(verbosity)
    {
        printf("Have read class names:");
	for(i=0;i<n_class_names;i++)
	{
	    printf(" %s",class_names[i]);
	}
	printf("\n");
    }

    /* Get attribute info */

    n_attributes = 0;
    attributes =(att_info*)malloc(11*sizeof(att_info));

    while(get_next_name(fp,1,1))
    {
	if(terminator!=':')
	{
	    printf("Attribute %s, not followed by :\n", name);
	    exit(1);
	}

	if(n_attributes&&(n_attributes%10==0)) 
	    attributes = (att_info*) realloc((void*)attributes,
				   (n_attributes+11)*sizeof(att_info));

	attributes[n_attributes] = (att_info)malloc(sizeof(struct _att_info));
	attribute = attributes[n_attributes];

	attribute->name = copy_string(name);

	if(!get_next_name(fp,0,1))
	{
	    printf("Information missing for attribute %s\n", name);
	    exit(1);
	}

	if(!strcmp(name,"ignore"))
	{
	    attribute->ignore = 1;
	    attribute->discrete = 0;
	    if(terminator!='\n') SkipComment;
	}
	else if(!strcmp(name,"continuous"))
	{
	    attribute->ignore = 0;
	    attribute->discrete = 0;
	    attribute->discrete_n = 0;
	    attribute->n_values = 0;
	    attribute->values = NULL;
	    get_type_name(attribute,fp);
	}
	else if(!strncmp(name,"discrete",8))
	{
	    attribute->ignore = 0;
	    attribute->discrete = 1;
	    attribute->discrete_n = atoi(name+8);
	    attribute->n_values = 0;
	    attribute->values=(char**)malloc(attribute->discrete_n*
						 sizeof(char*));
	    if(attribute->discrete_n && (attribute->values==NULL))
	    {
	        printf("Problem allocating space for values of %s\n",
		       attribute->name);
		exit(1);
	    }
	    get_type_name(attribute,fp);
	}
	else /* Presume that this is discrete with specified values */
	{
	    attribute->ignore = 0;
	    attribute->discrete = 1;

	    attribute->discrete_n = 0;
	    attribute->n_values = 0;
	    attribute->values = (char**)malloc(11*sizeof(char*));
	    do
	    {
	        if(attribute->n_values&&!(attribute->n_values%10))
		    attribute->values = (char**) realloc(
						     (void*)attribute->values,
				      (attribute->n_values+11)*sizeof(char*));
		attribute->values[attribute->n_values++] = copy_string(name);
		if(terminator!=',') break;
	    }
	    while(get_next_name(fp,0,1));

	    get_type_name(attribute,fp);
	}
	n_attributes++;
    }
    fclose(fp);


    /* Read the .data file, extracting constant occurrence info */

    for(i=0;i<n_attributes;i++)
    {
	attribute = attributes[i];
	if(attribute->discrete)
	    if(attribute->n_values)
	        attribute->value_occurs_tr = 
		  (int*) calloc((size_t)attribute->n_values,sizeof(int));
	    else
	        attribute->value_occurs_tr =
		  (int*) calloc((size_t)attribute->discrete_n,sizeof(int));
    }

    sprintf(filename,"%s.data",filestem);
    fp = fopen(filename,"r");

    if(!fp)
    {
        printf("Can't open %s\n", filename);
	exit(1);
    }

    if(verbosity) printf("Reading from %s\n", filename);

    cases = (char***)malloc(101*sizeof(char**));
    n_cases = 0;

    while(get_next_name(fp,1,attributes[0]->discrete))
    {
        if(n_cases&&!(n_cases%100))
	    cases = (char***) realloc((void*)cases,
				      (n_cases+101)*sizeof(char**));
	cases[n_cases] = (char**)malloc((n_attributes+1)*sizeof(char*));

	i = 0;

	do
	{
	    attribute = attributes[i];
	    if(attribute->ignore)
	    {
	    }
	    else if(!strcmp(name,"?")) 
	    {
	        cases[n_cases][i] = NULL;
	    }
	    else if(attribute->discrete)
	    {
	        for(j=0;j<attribute->n_values;j++)
		{
		    if(!strcmp(name,attribute->values[j])) break;
		}
		if(j==attribute->n_values) /* value not seen before */
		{
		    if(j>=attribute->discrete_n)
		    {
		        printf("%s has extra value %s in data file\n",
			       attribute->name, name);
			exit(1);
		    }
		    else /* add it in */
		    {
		        attribute->values[j] = copy_string(name);
			attribute->n_values++;
		    }
		}
		attribute->value_occurs_tr[j]++;
		cases[n_cases][i] = attribute->values[j];
	    }
	    else /* continuous attribute with value */
	    {
		cases[n_cases][i] = copy_string(name);
	    }
	    i++;
	}
	while(get_next_name(fp,1,(i==n_attributes) ?
			    1 : attributes[i]->discrete)
	      &&(i<n_attributes));
	if(i!=n_attributes)
	{
	    printf("Problem reading line in %s\n", filename);
	    exit(1);
	}
	for(j=0;j<n_class_names;j++)
	{
	    if(!strcmp(name,class_names[j])) break;
	}
	if(j==n_class_names)
	{
	    printf("Undeclared class name %s in data file\n", name);
	    exit(1);
	}
	cases[n_cases][i] = class_names[j];

	n_cases++;
    }
    fclose(fp);

    /* Read the .test file, extracting constant existence info */

    sprintf(filename,"%s.test",filestem);
    fp = fopen(filename,"r");

    if(fp&&verbosity) printf("Reading from %s\n", filename);

    testing_cases = (char***)malloc(101*sizeof(char**));
    n_testing_cases = 0;
    while(fp&&get_next_name(fp,1,attributes[0]->discrete))
    {
        if(n_testing_cases&&!(n_testing_cases%100))
	    testing_cases = (char***) realloc((void*)testing_cases,
                                        (n_testing_cases+101)*sizeof(char**));
	testing_cases[n_testing_cases] = (char**)malloc((n_attributes+1)*
							sizeof(char*));

	i = 0;

	do
	{
	    attribute = attributes[i];
	    if(attribute->ignore)
	    {
	    }
	    else if(!strcmp(name,"?")) 
	    {
	        testing_cases[n_testing_cases][i] = NULL;
	    }
	    else if(attribute->discrete)
	    {
	        for(j=0;j<attribute->n_values;j++)
		{
		    if(!strcmp(name,attribute->values[j])) break;
		}
		if(j==attribute->n_values) /* value not seen before */
		{
		    if(j>=attribute->discrete_n)
		    {
		        printf("%s has extra value %s in data file\n",
			       attribute->name, name);
			exit(1);
		    }
		    else /* add it in */
		    {
		        attribute->values[j] = copy_string(name);
			attribute->n_values++;
		    }
		}
		testing_cases[n_testing_cases][i]=attribute->values[j];
	    }
	    else /* continuous attribute with value */
	    {
		testing_cases[n_testing_cases][i] = copy_string(name);
	    }
	    i++;
	}
	while(get_next_name(fp,1,(i==n_attributes)?
			    1 : attributes[i]->discrete)
	      &&(i<n_attributes));
	if(i!=n_attributes)
	{
	    printf("Problem reading line in %s\n", filename);
	    exit(1);
	}
	for(j=0;j<n_class_names;j++)
	{
	    if(!strcmp(name,class_names[j])) break;
	}
	if(j==n_class_names)
	{
	    printf("Undeclared class name %s in data file\n", name);
	    exit(1);
	}
	testing_cases[n_testing_cases][i] = class_names[j];

	n_testing_cases++;
    }

    /* Now combine the attribute information into type information */

    n_types = 0;
    types = (type_info*) malloc(11*sizeof(type_info));

    for(i=0;i<n_attributes;i++)
    {
        attribute = attributes[i];
	if(attribute->ignore) continue;
	for(j=0;j<n_types;j++)
	{
	    if(!strcmp(types[j]->name,attribute->type_name))
	        break;
	}
	attribute->type_number = j;
	if(j==n_types) /* New type */
	{
            if(n_types&&!(n_types%10))
	        types = (type_info*) realloc((void*)types, 
					     (n_types+11)*sizeof(type_info));
	    types[n_types] = (type_info)malloc(sizeof(struct _type_info));
	    types[n_types]->name = attribute->type_name;
	    types[n_types]->discrete = attribute->discrete;
	    types[n_types]->n_atts = 0;
	    types[n_types]->atts = (int*)malloc(11*sizeof(int));
	    types[n_types]->n_values = 0;
	    if(types[n_types]->discrete)
	    {
	        types[n_types]->values = (char**)malloc(11*sizeof(char*));
		types[n_types]->value_occurs_tr =(int*)malloc(11*sizeof(int*));
	    }
	    else
	    {
	        types[n_types]->values = NULL;
	    }
            n_types++;
	}
	type = types[j];
	if(type->n_atts&&!(type->n_atts%10))
	    type->atts = (int*) realloc((void*)type->atts,
					(type->n_atts+11)*sizeof(int));
	type->atts[type->n_atts++] = i;

	if(type->discrete!=attribute->discrete)
	{
	    printf("Type %s declared to be both discrete and continuous\n",
		   type->name);
	    exit(1);
	}

	if(type->discrete)
	{
	    for(j=0;j<attribute->n_values;j++)
	    {
	        for(k=0;k<type->n_values;k++)
		{
		    if(!strcmp(attribute->values[j],type->values[k]))
		        break;
		}
		if(k==type->n_values) /* New value for this type */
		{
		    if(type->n_values&&!(type->n_values%10))
		    {
		        type->values = (char**) realloc((void*)type->values,
					    (type->n_values+11)*sizeof(char*));
			type->value_occurs_tr =(int*)realloc(
						 (void*)type->value_occurs_tr,
					    (type->n_values+11)*sizeof(int*));
		    }
		    type->values[k] = attribute->values[j];
		    type->value_occurs_tr[k] = 0;
		    type->n_values++;
		}
		type->value_occurs_tr[k] += attribute->value_occurs_tr[j];
	    }
	}
    }

    /* Check the discrete type names to ensure that none is the prefix of
       another */
    for(i=0;i<n_types;i++)
    {
        if(!type->discrete) continue;
	for(j=i+1;j<n_types;j++)
	{
	    if(!type->discrete) continue;
	    if((int)strlen(types[i]->name)>(int)strlen(types[j]->name))
	    {
	        if(!strncmp(types[i]->name,types[j]->name,
			    strlen(types[j]->name)))
		{
		    printf("Type name %s is prefix of type name %s\n",
			   types[j]->name, types[i]->name);
		    exit(1);
		}
	    }
	    else
	    {
	        if(!strncmp(types[j]->name,types[i]->name,
			    strlen(types[i]->name)))
		{
		    printf("Type name %s is prefix of type name %s\n",
			   types[i]->name, types[j]->name);
		    exit(1);
		}
	    }
	}
    }

    /* Output type info on std out */
    if(verbosity)
    {
        printf("Type information from names file:\n\n");
	for(i=0;i<n_types;i++)
	{
	    type = types[i];
	    printf("%s:\n",type->name);
	    if(type->discrete) printf("\tdiscrete\n");
	    else printf("\tcontinuous\n");
	    printf("\tattributes:\n");
	    for(j=0;j<type->n_atts;j++)
	        printf("\t\t%s\n",attributes[type->atts[j]]->name);
	    printf("\tvalues:\n");
	    for(j=0;j<type->n_values;j++)
	        printf("\t\t%s\n",type->values[j]);
	}
    }

    /* Now write out to the .d file for FOIL */

    sprintf(filename,"%s.d",filestem);
    tp = fopen(filename,"w");

    if(verbosity) printf("Writing to %s\n", filename);

    /* First the types complete with constants - note that in the .d file each 
       discrete constant name is augmented by being preceded by its type
       name to prevent FOIL equating two constants of different types */

    for(i=0;i<n_types;i++)
    {
	type = types[i];
	if(type->discrete)
	{
	    fprintf(tp,"#%s: ",type->name);
	    k = 0;
	    for(j=0;j<type->n_values;j++)
	    {
	        if(k) fprintf(tp,", ");
		if(type->value_occurs_tr[j])
		    fprintf(tp,"*"); /* Theory Constant */
		fprintf(tp,"%s%s",type->name, type->values[j]);
		k++;
	    }
	    if(!k) fprintf(tp,"\n"); /* empty type */
	    fprintf(tp,".\n");
	}
	else
	    fprintf(tp,"%s: continuous.\n",type->name);
    }
    fprintf(tp,"\n");

    /* Now the sole relation - is_first_named_class() */

    fprintf(tp,"is_%s(", class_names[0]);

    k = 0;
    for(i=0;i<n_attributes;i++)
    {
        attribute = attributes[i];
        if(attribute->ignore) continue;
        if(k) fprintf(tp,",");
        fprintf(tp,"%s",attribute->type_name);
	k++;
    }
    if(!k)
    {
        printf("All attributes ignored\n");
	exit(1);
    }
    fprintf(tp,")\n");

    /* Now the positive training cases */

    for(i=0;i<n_cases;i++)
    {
        if(cases[i][n_attributes]!=class_names[0]) continue;

	k = 0;
	for(j=0;j<n_attributes;j++)
	{
	    attribute = attributes[j];
	    if(attribute->ignore) continue;

	    if(k) fprintf(tp,",");

	    if(!cases[i][j]) /* Missing Value */
	    {
	        fprintf(tp,"?");
	    }
	    else if(attribute->discrete)
	    {
	        fprintf(tp,"%s%s",attribute->type_name,cases[i][j]);
	    }
	    else /* attribute is continuous */
	    {
	        fprintf(tp,"%s",cases[i][j]);
	    }
	    k++;
	}
	fprintf(tp,"\n");
    }

    /* Now the negative training cases */

    fprintf(tp,";\n");
    for(i=0;i<n_cases;i++)
    {
        if(cases[i][n_attributes]==class_names[0]) continue;

	k = 0;
	for(j=0;j<n_attributes;j++)
	{
	    attribute = attributes[j];
	    if(attribute->ignore) continue;

	    if(k) fprintf(tp,",");
	    if(!cases[i][j]) /* Missing Value */
	    {
	        fprintf(tp,"?");
	    }
	    else if(attribute->discrete)
	    {
	        fprintf(tp,"%s%s",attribute->type_name,cases[i][j]);
	    }
	    else /* attribute is continuous */
	    {
	        fprintf(tp,"%s",cases[i][j]);
	    }
	    k++;
	}
	fprintf(tp,"\n");
    }
    fprintf(tp,".\n");

    /* Now the test cases */

    if(!fp)
    {
	fclose(tp);
	exit(0);
    }

    fprintf(tp,"\nis_%s\n", class_names[0]);

    for(i=0;i<n_testing_cases;i++)
    {
	k = 0;
	for(j=0;j<n_attributes;j++)
	{
	    attribute = attributes[j];
	    if(attribute->ignore) continue;

	    if(k) fprintf(tp,",");
	    if(!testing_cases[i][j]) /* Missing Value */
	    {
	        fprintf(tp,"?");
	    }
	    else if(attribute->discrete)
	    {
	        fprintf(tp,"%s%s",attribute->type_name,testing_cases[i][j]);
	    }
	    else /* attribute is continuous */
	    {
	        fprintf(tp,"%s",testing_cases[i][j]);
	    }
	    k++;
	}
        if(testing_cases[i][n_attributes]==class_names[0])
	    fprintf(tp,":+\n");
	else
	    fprintf(tp,":-\n");
    }
    fprintf(tp,".\n");
    fclose(fp);
    fclose(tp);

    return 0; /* Changed from exit(0) */
}

/*****************************************************************************/
/* copy_string(s)  - return a copy of the string s                           */
/*****************************************************************************/

char *copy_string(char *s)
{
    char *tmp;

    tmp = (char*)malloc((strlen(s)+1)*sizeof(char));
    return strcpy(tmp,s);
}

/*****************************************************************************/
/* get_next_name(fp,skip,embed) - read next name from file *fp into name     */
/*                  Modified version from FOIL's ReadName                    */
/*                  skip is !0 if trailing comment to be skipped             */
/*                  embed is !0 if need to escape an embedded period         */
/*****************************************************************************/

#define Space(c)        (c == ' ' || c == '\t' || c == '\n')

int  get_next_name(FILE *fp, int skip, int embed)
/*   ---------  */
{
    char *Sp = name;
    int c;

    /*  Skip to first non-space character  */

    while ( ( c = getc(fp) ) == '|' || Space(c) )
	if ( c == '|' ) SkipComment;

    /*  Return 0 if no names to read  */

    if ( c == EOF )
    {
	return 0;
    }

    /*  Read in characters up to the next delimiter  */

    while ( c != ',' && c != '\n' && c != '|' && c != EOF && c != ':')
    {
        if ( c == '.' )
	{
	    if(c=getc(fp))
	    {
	        if( Space(c) || c=='|' || c==EOF )
		{
		    c = '.';
		    break;
		}
	    }
	    else
	    {
	        c = '.';
		break;
	    }
	    if(embed) *Sp++ = '\\'; /* Need to escape embedded . */
	    *Sp++ = '.';
	}

	if ( c == '\\' ) 
	{
	    *Sp++ = (char) c; /* Perpetuate embedded characters for FOIL */
	    c = getc(fp);
	}

	*Sp++ = (char) c;
	if ( c == ' ' )
	    while ( ( c = getc(fp) ) == ' ' );
	else
	    c = getc(fp);
    }

    terminator = c;

    /* Skip trailing comment? */

    if(skip && (c == '|'))
    {
        SkipComment;
    }

    /*  Strip trailing spaces  */

    while ( Space(*(Sp-1)) ) Sp--;
    *Sp++ = '\0';

    return 1;
}

/*****************************************************************************/
/* get_type_name(attribute,fp) - get the type name for attribute from fp     */
/*****************************************************************************/

void get_type_name(att_info attribute, FILE *fp)
{
    while(terminator!='|'&&terminator!='\n'&&terminator!=EOF)
    {
        terminator = getc(fp);
    }

    if(terminator=='|'&&get_next_name(fp,1,1)&&!strcmp(name,"type")
       &&get_next_name(fp,1,1))
    {
        attribute->type_name = copy_string(name);
    }
    else
    {
        printf("Attribute %s lacks type info\n", attribute->name);
	exit(1);
    }

    return;
}
