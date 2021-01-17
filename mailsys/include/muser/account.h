#ifndef _MUSER_ACCOUNT_H
#define _MUSER_ACCOUNT_H

#include<msys/mtypes.h>

#define ACNTSIZE sizeof(struct u_account)
#define _uno_to_cno(__uno)  (__uno - (1 << 22))
#define _cno_to_uno(__cno)  (__cno + (1 << 22))

struct u_account {
    Numb_t usernumb;/*user's account number  */
    Name_t username;/*user's account name    */
    Pswd_t password;/*user's account password*/
    Setting usr_set;/*user's setting bit map */
    Frnd_t userfrnd;/*user's friends - array */
    Bool_t isexistd;/*flags about user exist */
    Nums_t frndnums;/*nums of user's friends */
};
#endif