#include "searchparams.h"

SearchParams::SearchParams() {
    setKeywords(QString("placeholder"));
    m_sortBy = SortByRelevance;
}
