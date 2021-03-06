#ifndef MST_H
#define MST_H

#include "vertex.h"

void MST(const uint32_t & NT,
         const index_t & NV,
         const index_t & NE,
         const Edge * pE,
         const Edge ** pMST);

#endif /* MST_H */
