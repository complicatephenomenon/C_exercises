#include <string>
#include <iostream>
using namespace std;

class Line {
   public:
      double length;
      string name;
      void setLength( double len );
      double getLength( void );
      string setname(string eman);
};

// Member functions definitions
double Line::getLength(void) {
   return length ;
}

void Line::setLength( double len) {
   length = len;
}

string Line:: setname(string eman){
    name = eman;
}

/**
 * When you type using namespace std, you are taking everything inside of * the namespace std and moving it to the global scope, so that you can
 * use the shorter cout instead of the more fully-qualified std::cout.
 */
int main()
{
    string result;
    string s1 = "hello";
     // without the extra space at the end
    string s2 = "world";
    result = s1;
    result += ' ';
     // append a space at the end
    result += s2;
    Line line;

    // set line length
    line.setLength(6.0);
    line.setname("Straight Line");
    cout << "Length of line : " << line.getLength() << endl;

    // set line length without member function
    line.length = 10.0; // OK: because length is public
    cout << "Length of line : " << line.length << endl;
    cout << "name of line : " << line.name << endl;

    Line *ptr_line;
    ptr_line = & line;
    cout << ptr_line->name << "!" << endl;

    return 0;
}
