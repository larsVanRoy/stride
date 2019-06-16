/*
 *  This is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  The software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with the software. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2018, Jan Broeckhove and Bistromatics group.
 */

#include "EpiReaderFactory.h"
#include "EpiJSONReader.h"
#include "util/Exception.h"

#include <memory>
#include <string>
#include <fstream>

#ifdef BOOST_FOUND
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
namespace filesys = boost::filesystem;
#else
#include <filesystem>
namespace filesys = std::filesystem;
#endif


namespace geopop {

std::shared_ptr<EpiReader> EpiReaderFactory::CreateEpiReader(const std::string& filename) {

    const filesys::path path(filename);
    if (!filesys::exists(path)) {
        throw stride::util::Exception("GeoGridReaderFactory::CreateReader> File not found: " + path.string());
    }
//    std::unique_ptr<std::ifstream> stream;
//    stream->open(filename);

    if (path.extension().string() == ".json") {
        return std::make_shared<EpiJSONReader>(std::make_unique<std::ifstream>(path.string()));
    }
//        else if (path.extension().string() == ".proto") {
//            return std::make_shared<EpiJSONWriter>();
//        }
//        else if (path.extension().string() == ".h5") {
//            return std::make_shared<EpiJSONWriter>();
//        }
    else {
        throw stride::util::Exception("EpiWriterFactory::CreateWriter> Unsupported file extension: " +
                                      path.extension().string());
    }
}

} // namespace geopop
