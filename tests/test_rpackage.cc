#include "config.h" // IWYU pragma: associated

#include "rpackage.h"
#include "rpackagelister.h"

#include <apt-pkg/configuration.h>
#include <apt-pkg/init.h>
#include <apt-pkg/pkgsystem.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char **argv)
{
   pkgInitConfig(*_config);
   pkgInitSystem(*_config, _system);

   RPackageLister *lister = new RPackageLister();
   lister->openCache();

   RPackage *pkg = lister->getPackage("build-essential");
   if (pkg) {
      cerr << "pkg: " << pkg->name() << endl;
      vector<DepInformation> deps = pkg->enumDeps();
      for (unsigned int i = 0; i < deps.size(); i++) {
         cerr << "deps: " << deps[i].name << endl;
      }
      cerr << "size: " << deps.size() << endl;
   } else {
      cerr << "There is no package named build-essential" << endl;
   }
}
