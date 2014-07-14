#ifndef OCT_DATA_H
#define OCT_DATA_H

////#include "boost/serialization/access.hpp"
////#include "boost/serialization/serialization.hpp"
////#include "boost/serialization/array.hpp"
//
//class oct_volume
//{
//
//    friend class boost::serialization::access;
//
//public:
//
//    double x_range;
//    double y_range;
//    double z_range;
//
//    short x_steps;
//    short y_steps;
//    short z_steps;
//
//    short *data;
//
//    template <typename Archive>
//    void serialize(Archive& ar, const unsigned int version)
//    {
//      ar & x_range & y_range & z_range;
//      ar & x_steps & y_steps & z_steps;
//      const size_t data_size = x_steps*y_steps*z_steps*sizeof(short);
//      //ar & boost::serialization::make_array(this->data,data_size);
//
//    }
//
//
//
//public:
//
//    oct_volume() : x_range(0.0) , y_range(0.0) , z_range(0.0), x_steps(0), y_steps(0), z_steps(0),data(new short[256*256]) {}
//
//    oct_volume(double x_rang, double y_rang, double z_rang, int x_step, int y_step, int z_step) : x_range(x_rang),y_range(y_rang),z_range(z_rang),
//        x_steps(x_step), y_steps(y_step), z_steps(z_step),data(new short[x_step*y_step*z_step]){
//
//        int size = 5;
//    }
//
//};


#endif // OCT_DATA_H
