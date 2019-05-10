    #include <POI.h>
    POI::POI(poi_type type)
    {
        this->type = type;
    }
    POI::~POI()
    {
        for(int i = 0; i<tags.size();i++)
            delete tags[i];
        tags.clear();
    }
    void POI::addTag(Tag *t)
    {
        tags.push_back(t);
    }
    void POI::addTag(string name, string desc)
    {
        Tag *t = new Tag;
        t->name = name;
        t->desc = desc;
        tags.push_back(t);
    }