#include <stdint.h>
#include <string.h>
#include <gmp.h>
#include "rsa.h"

const char* const p_str = "14299623962416399520070177382898895550795403345466153217470516082934737582776038882967213386204600674145392845853859217990626450972452084065728686565928113";
const char* const q_str = "7630979195970404721891201847792002125535401292779123937207447574596692788513647179235335529307251350570728407373705564708871762033017096809910315212884101";
const char* const d_str = "46730330223584118622160180015036832148732986808519344675210555262940258739805766860224610646919605860206328024326703361630109888417839241959507572247284807035235569619173792292786907845791904955103601652822519121908367187885509270025388641700821735345222087940578381210879116823013776808975766851829020659073";
static mpz_t p, q, d, u, mod, dp, dq;


void rsa_init(void)
{
	mpz_set_str(p, p_str, 10);
	mpz_set_str(q, q_str, 10);
	mpz_set_str(d, d_str, 10);

	mpz_t pm1,qm1;
	mpz_init2(pm1, 520);
	mpz_init2(qm1, 520);

	mpz_sub_ui(pm1, p, 1);
	mpz_sub_ui(qm1, q, 1);
	mpz_invert(u, p, q);
	mpz_mod(dp, d, pm1);
	mpz_mod(dq, d, qm1);

	mpz_mul(mod, p, q);

	mpz_clear(pm1);
	mpz_clear(qm1);
}

void rsa_term(void)
{

}

void rsa_decrypt(uint8_t buffer[128])
{
	mpz_t c,v1,v2,u2,tmp;
	mpz_init2(c, 1024);
	mpz_init2(v1, 1024);
	mpz_init2(v2, 1024);
	mpz_init2(u2, 1024);
	mpz_init2(tmp, 1024);

	mpz_import(c, 128, 1, 1, 0, 0, buffer);

	mpz_mod(tmp, c, p);
	mpz_powm(v1, tmp, dp, p);
	mpz_mod(tmp, c, q);
	mpz_powm(v2, tmp, dq, q);
	mpz_sub(u2, v2, v1);
	mpz_mul(tmp, u2, u);
	mpz_mod(u2, tmp, q);
	if(mpz_cmp_si(u2, 0) < 0){
		mpz_add(tmp, u2, q);
		mpz_set(u2, tmp);
	}
	mpz_mul(tmp, u2, p);
	mpz_set_ui(c, 0);
	mpz_add(c, v1, tmp);

	size_t count = (mpz_sizeinbase(c, 2) + 7)/8;
	memset(buffer, 0, 128 - count);
	mpz_export(&buffer[128 - count], NULL, 1, 1, 0, 0, c);

	mpz_clear(c);
	mpz_clear(v1);
	mpz_clear(v2);
	mpz_clear(u2);
	mpz_clear(tmp);
}



