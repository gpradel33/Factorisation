#include "factorisation.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gmp.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>

#define TAILLE 10000
#define FRIABLE 100

unsigned long int*
crible_erat()
{
  unsigned long int* tab = calloc(TAILLE, sizeof(unsigned long int));
  unsigned long int* tab2 = malloc(TAILLE*sizeof(unsigned long int));
  int k = 0;

  for(unsigned long int i = 2; i < TAILLE; i++)
    {
      if(tab[i] == 0)
	{
	  tab[i] = 1;
	  tab2[k] = i;
	  k++;
	  for(int j = i*2; j < TAILLE; j=j+i)
	    tab[j] = -1;
	}
    }
  
  free(tab);
  return tab2;
}



// fonction ultra-naïve de factorisation d'entier

/*void factorisation(mpz_t n, int B)
{
  mpz_t tmp, div, q, r, root;
  unsigned int cnt2 = 0;
  unsigned long int cnt = 0;
  mpz_init(tmp);
  mpz_set(tmp, n);
  mpz_init(div);
  mpz_set_ui(div, 2);
  mpz_init(root);
  mpz_sqrt(root, n);
  mpz_add_ui(root, root, 1);
  int nb_primes = 0;
  unsigned long int* prime_nbs = crible_erat(B);
  unsigned long int root_ui = mpz_get_ui(root);
  mpz_t* list_factor = malloc(root_ui*sizeof(mpz_t));

  if (list_factor == NULL)
    {
      fprintf (stderr, "Malloc Problem\n");
      exit(EXIT_FAILURE);
    }
  
  for(unsigned long int i = 0; i < root_ui; i++)
    mpz_init(list_factor[i]);

  mpz_init(r);
  mpz_init(q);

  while(prime_nbs[nb_primes] < B)
    nb_primes++;
  
  while(mpz_cmp_ui(tmp, 1) != 0)
    {
      mpz_tdiv_qr(q, r, tmp, div);
      if (mpz_cmp_ui(r, 0) == 0)
	{
	  mpz_set(tmp, q);
	  mpz_set(list_factor[cnt], div);
	  cnt++;
	}
      else
	{
	  if (cnt2 < nb_primes)
	    {
	      mpz_set_ui(div, prime_nbs[cnt2]);
	      cnt2++;
	    }
	  else
	    mpz_add_ui(div, div, 1);
	}
    }

  for (unsigned long int i = 0; i < cnt; i++)
    gmp_printf("%Zd ", list_factor[i]);
  gmp_printf("\n");
  
  mpz_clear(tmp);
  mpz_clear(div);
  //mpz_clear(cnt);
  for(unsigned long int i = 0; i < root_ui; i++)
    mpz_clear(list_factor[i]);
  free(list_factor);
  mpz_clear(r);
  mpz_clear(q);
  mpz_clear(root);
  free(prime_nbs);
  return list_factor;
}
*/
// Problème de segmentation avec l'entier 49392123903


void step_pollard(mpz_t n, mpz_t factor)
{
  //Initialisation de ce dont on a besoin
  mpz_t pgcd;
  mpz_init(pgcd);
  mpz_set_ui(pgcd,1);
  mpz_t rest;
  mpz_init(rest);
  mpz_t cnt;
  mpz_init(cnt);
  mpz_t rand_minus_one;
  mpz_init(rand_minus_one);
  int cnt2 = 0;
   mpz_set_ui(factor,0);
  //Initialisation de l'algo
  unsigned long int time_tmp;
  struct timeval t_tmp;
  gettimeofday(&t_tmp, NULL);
  time_tmp = t_tmp.tv_sec*1000000 + t_tmp.tv_usec;
  mpz_t rand;
  mpz_init(rand);  
  gmp_randstate_t state;
  gmp_randinit_default(state);
  gmp_randseed_ui(state, time_tmp);
  
  mpz_set_ui(rand, 2);
  mpz_cdiv_r_ui(rest, n, 2);
  if(mpz_cmp_ui(rest, 0)==0)
    mpz_set_ui(rand,3);
  
  while(cnt2 < FRIABLE && mpz_cmp_ui(factor, 0)==0)
      {
	if (mpz_cmp_ui(rand,0)==0 || mpz_cmp_ui(rand, 1)==0)
	  mpz_urandomm(rand, state, n);
	mpz_set_ui(cnt, 2);
	//gmp_printf("Rand: %Zd\n", rand);
	while(cnt2 < FRIABLE && mpz_cmp_ui(pgcd, 1)==0)
	{
	  if (mpz_cmp_ui(rand,1)==0 || mpz_cmp_ui(rand, 0)==0)
	    mpz_urandomm(rand, state, n);
	  mpz_sub_ui(rand_minus_one, rand, 1);
	  mpz_gcd(pgcd,rand_minus_one,n);
	  //gmp_printf("PGCD %Zd\n", pgcd);
	  if(mpz_cmp_ui(pgcd,1)!=0)
	    {
	      //gmp_printf("%Zd ", factor);
	      mpz_set(factor, pgcd);
	    }
	  mpz_powm(rand, rand, cnt, n);
	  mpz_add_ui(cnt, cnt, 1);
	  //gmp_printf("2ème boucle Rand %Zd\n", rand);
	  cnt2++;
	}
    }

  //Aucun facteur trouvé. Sûrement premier ou échec de l'algorithme. On renvoie le même nombre.
  if(mpz_cmp_ui(factor,0)==0)
    mpz_set(factor,n);
  gmp_printf("Par pollard: un facteur de %Zd est %Zd \n", n, factor);
  mpz_clear(rand);
  mpz_clear(pgcd);
  mpz_clear(rest);
  mpz_clear(cnt);
  mpz_clear(rand_minus_one);
  gmp_randclear(state);
  printf("FIN DE STEP POLLARD\n");
}

void pollard(mpz_t n)
{
  printf("POLLARD ALGORITHM \n");
  mpz_t tmp, root, factor, div, check;
  mpz_init(tmp);
  mpz_set(tmp,n);
  mpz_init(factor);
  mpz_init(root);
  mpz_init(check);
  mpz_init(div);
  mpz_sqrt(root,n);
  mpz_add_ui(root,root,1);
  unsigned long int root_ui = mpz_get_ui(root);
  unsigned long int cnt = 0;
  int cnt2 = 0;
  mpz_t* list_factor = malloc(root_ui*sizeof(mpz_t));
  for(unsigned long int i = 0; i < root_ui; i++)
    mpz_init(list_factor[i]);

  while(mpz_cmp_ui(tmp,1)!=0)
    {
      //gmp_printf("tmp = %Zd\nfactor = %Zd \n", tmp, factor);
      step_pollard(tmp, factor);
      //printf("STEP POLLARD POUR CHECK\n");
      //step_pollard(factor, check);
      //gmp_printf("check = %Zd\n", check);
      //gmp_printf("Au début factor %Zd\n", factor);
      while(cnt2 < FRIABLE && mpz_probab_prime_p(factor, 47)==0)
	{
	  //gmp_printf("Avant deuxieme step factor %Zd\n", factor);
	  mpz_set(check, factor);
	  step_pollard(check, factor);
	  // gmp_printf("Factor dans boucle premier %Zd\n", factor);
	  //mpz_cdiv_q(factor, factor, check);
	  //mpz_set(check,factor);
	  //step_pollard(check, check);
	  cnt2++;
	}
      
      //gmp_printf("2 tmp = %Zd \n factor = %Zd \n", tmp, factor);
      mpz_cdiv_q(div, tmp, factor);
      mpz_set(list_factor[cnt], factor);
      mpz_set(tmp, div);
      //gmp_printf("\n AFTER tmp = %Zd factor = %Zd div = %Zd\n",tmp, factor, div);
      cnt++;
    }
  
  for (unsigned long int i = 0; i < cnt; i++)
    gmp_printf("%Zd ", list_factor[i]);
  gmp_printf("\n");
  
  for(unsigned long int i = 0; i < root_ui; i++)
    mpz_clear(list_factor[i]);
  free(list_factor);
  mpz_clear(tmp);
  mpz_clear(root);
  mpz_clear(factor);
  mpz_clear(check);
  mpz_clear(div);
  
  }

mpz_t*
friable(int a, mpz_t n, int C)
{
  int nb_primes = 0;
  int k = 0;
  unsigned long int B = C;
  unsigned long int m = mpz_get_ui(n);
  unsigned long int* prime_nbs = crible_erat();
  
  while(prime_nbs[nb_primes] < B)
    nb_primes++;

  mpz_t* smooth_list = malloc((nb_primes+1)*sizeof(mpz_t));

  for(int l = 0; l < nb_primes+1; l++)
    mpz_init(smooth_list[l]);
  
  while(k < nb_primes)
    {
      if(m%prime_nbs[k] == 0)
	{
	  m = m/prime_nbs[k];
	  mpz_add_ui(smooth_list[k+1], smooth_list[k+1], 1);
	}
      else
	k++;
    }
  
  if(m == 1)
    {
      mpz_set_ui(smooth_list[0], 1);
      printf("%d-FRIABLE\n", C);
      for(int l = 1; l < nb_primes+1; l++)
	if(mpz_cmp_ui(smooth_list[l], 0)!=0)
	  gmp_printf("[%d, %Zd]\n", prime_nbs[l-1], smooth_list[l]);
    }
  else
    {
      mpz_set_ui(smooth_list[0], 0);
      printf("NON-FRIABLE\n");
      if(a == 0)
	{
	  mpz_t* list_factor = malloc(m*sizeof(mpz_t));
	  for(unsigned long int i = 0; i < m; i++)
	    mpz_init(list_factor[i]);
	  unsigned long int cnt = prime_nbs[nb_primes - 1] + 1;
	  int cnt2 = 0;
	  while(m != 1)
	    {
	      if(m%cnt == 0)
		{
		  m = m/cnt;
		  mpz_add_ui(list_factor[cnt2], list_factor[cnt2], 1);
		}
	      else
		{
		  cnt++;
		  cnt2++;
		}
	    }
	  for(int l = 1; l < nb_primes+1; l++)
	    if(mpz_cmp_ui(smooth_list[l], 0)!=0)
	      gmp_printf("[%d, %Zd]\n", prime_nbs[l-1], smooth_list[l]);
	  for(int l = 0; l < cnt2+1; l++)
	    if(mpz_cmp_ui(list_factor[l], 0)!=0)
	      gmp_printf("[%d, %Zd]\n", l+1+prime_nbs[nb_primes-1], list_factor[l]);

	  for(unsigned long int l = 0; l < m; l++)
	    mpz_clear(list_factor[l]);
	  free(list_factor);
	}
      if(a == 1)
	{
	  for(int l = 1; l < nb_primes+1; l++)
	    if(mpz_cmp_ui(smooth_list[l], 0)!=0)
		gmp_printf("[%d, %Zd]\n", prime_nbs[l-1], smooth_list[l]);
	  printf("[%lu, ~]\n", m);
	}
    }
  free(prime_nbs);
  return smooth_list;
}

void dixon(mpz_t n)
{
  int nb_primes = 0;
  int cnt = 0;
  unsigned long int* prime_nbs = crible_erat();
  mpz_t rand;
  mpz_init(rand);
  mpz_t sq_rand;
  mpz_init(sq_rand);
  unsigned long int time_tmp;
  struct timeval t_tmp;
  gmp_randstate_t state;
  gmp_randinit_default(state);

  while(prime_nbs[nb_primes] < FRIABLE)
    nb_primes++;
  free(prime_nbs);
  mpz_t* smooth_list;
  
  unsigned long int** tab = malloc(nb_primes*sizeof(unsigned long int*));
  
  for(int j = 0; j < nb_primes; j++)
    tab[j] = malloc((nb_primes+1)*sizeof(unsigned long int)); 

  for(int a = 0; a < nb_primes; a++)
    for(int b = 0; b <= nb_primes; b++)
      tab[a][b] = 58;
  
  while(cnt <= nb_primes)
    {
      gettimeofday(&t_tmp, NULL);
      time_tmp = t_tmp.tv_sec*1000000 + t_tmp.tv_usec;
      gmp_randseed_ui(state, time_tmp);
      mpz_urandomm(rand, state, n);
      mpz_mul(sq_rand, rand, rand);
      mpz_mod(sq_rand, sq_rand, n);
      smooth_list = friable(0, sq_rand, FRIABLE);
      if(mpz_cmp_ui(smooth_list[0], 1)==0)
	{
	  for(int j = 0; j < nb_primes; j++)
	    tab[j][cnt] = mpz_get_ui(smooth_list[j+1]); 
	  cnt++;
	}
      for(int j = 0; j < nb_primes+1; j++)
	mpz_clear(smooth_list[j]);
      free(smooth_list);
    }
  
  for(int k = 0; k < nb_primes; k++)
    {
      for(int l = 0; l <= nb_primes; l++)
  	printf("%lu  ", tab[k][l]);
      printf("\n");
    }
  
  for(int k = 0; k < nb_primes; k++)
    free(tab[k]);
  free(tab);

  mpz_clear(rand);
  mpz_clear(sq_rand);
  gmp_randclear(state);
}

int
main(int argc, char *argv[])
{
  if(argc < 2)
    {
      printf("Veuillez mettre un nombre à factoriser.\n");
      return EXIT_FAILURE;
    }
  
  mpz_t n;
  mpz_init(n);
  mpz_set_ui(n, atoi(argv[1]));
  mpz_t* list = friable(0, n, FRIABLE);
  for(int k = 0; k < 26; k++)
    mpz_clear(list[k]);
  free(list);
  //dixon(n);
  //printf("Par l'algorithme naïf de factorisation, nous obtenons:\n");
  //factorisation(n);
  //printf("Par l'algorithme p-1 de Pollard, nous obtenons:\n");
  //pollard(n);

  mpz_clear(n);
  return EXIT_SUCCESS;
}

