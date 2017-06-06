#include "color.hpp"

using namespace tinyxml2;

COLOR loadColorFromXMLNode(XMLNode *tnode)
{
    COLOR tcol;

    XMLNode *anode = NULL;

    anode = tnode->FirstChild();


    while(anode != NULL)
    {
        if(!strcmp(anode->Value(), "bg")) anode->ToElement()->QueryIntText(&tcol.m_Background);
        else if(!strcmp(anode->Value(), "fg")) anode->ToElement()->QueryIntText(&tcol.m_Foreground);
        else if(!strcmp(anode->Value(), "bold"))
        {
            if( !strcmp(anode->ToElement()->GetText(), "true")) tcol.m_Bold = true;
        }

        anode = anode->NextSibling();
    }

    return tcol;
}
