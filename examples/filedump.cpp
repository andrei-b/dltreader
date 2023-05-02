#include "directdltfile.h"
#include "dltfileparser.h"
#include "transferredfiles.h"
#include <stdio.h>


using namespace std;

int main(int argc, char ** argv)
{
    if (argc == 1) {
        printf("This program dumps files attached to the given dlt file via file transfer.\n");
        return 0;
    }
    DLTReader::DirectDLTFile f(argv[1]);
    DLTReader::TransferredFiles<DLTReader::DLTFileRecordIterator> files(f.begin(), f.end());
    while (files.findFile()) {
        printf("%s %i %s\n", files.currentFileName().data(), files.currentFileSize(), files.currentFileDate().data());
        auto contents = files.getCurrentFileContents();
        printf("%s\n", contents.data());
    }
    return 0;
}
