///////////////////////////////////////////////////////////////////////////////
// $Id: test_gdal.cpp,v 1.4 2006/12/06 15:39:13 mloskot Exp $
//
// Project:  C++ Test Suite for GDAL/OGR
// Purpose:  Test general GDAL features.
// Author:   Mateusz Loskot <mateusz@loskot.net>
// 
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2006, Mateusz Loskot <mateusz@loskot.net>
//  
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the
// Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.
///////////////////////////////////////////////////////////////////////////////
//
//  $Log: test_gdal.cpp,v $
//  Revision 1.4  2006/12/06 15:39:13  mloskot
//  Added file header comment and copyright note.
//
//
///////////////////////////////////////////////////////////////////////////////
#include <tut.h>
#include <gdal.h>
#include <gdal_priv.h>
#include <gdal_utils.h>
#include <string>
#include <limits>

namespace tut
{
    // Common fixture with test data
    struct test_gdal_data
    {
        // Expected number of drivers
        int drv_count_;

        test_gdal_data()
            : drv_count_(0)
        {
            // Windows CE port builds with fixed number of drivers
#ifdef FRMT_aaigrid
            drv_count_++;
#endif
#ifdef FRMT_dted
            drv_count_++;
#endif
#ifdef FRMT_gtiff
            drv_count_++;
#endif
        }
    };

    // Register test group
    typedef test_group<test_gdal_data> group;
    typedef group::object object;
    group test_gdal_group("GDAL");

    // Test GDAL driver manager access
    template<>
    template<>
    void object::test<1>()
    {
        GDALDriverManager* drv_mgr = NULL;
        drv_mgr = GetGDALDriverManager();
        ensure("GetGDALDriverManager() is NULL", NULL != drv_mgr);
    }

    // Test number of registered GDAL drivers
    template<>
    template<>
    void object::test<2>()
    {
#ifdef WIN32CE
        ensure_equals("GDAL registered drivers count doesn't match",
            GDALGetDriverCount(), drv_count_);
#endif
    }

    // Test if AAIGrid driver is registered
    template<>
    template<>
    void object::test<3>()
    {
        GDALDriverH drv = GDALGetDriverByName("AAIGrid");

#ifdef FRMT_aaigrid
        ensure("AAIGrid driver is not registered", NULL != drv);
#else
        (void)drv;
        ensure(true); // Skip
#endif
    }

    // Test if DTED driver is registered
    template<>
    template<>
    void object::test<4>()
    {
        GDALDriverH drv = GDALGetDriverByName("DTED");

#ifdef FRMT_dted
        ensure("DTED driver is not registered", NULL != drv);
#else
        (void)drv;
        ensure(true); // Skip
#endif
    }

    // Test if GeoTIFF driver is registered
    template<>
    template<>
    void object::test<5>()
    {
        GDALDriverH drv = GDALGetDriverByName("GTiff");

#ifdef FRMT_gtiff
        ensure("GTiff driver is not registered", NULL != drv);
#else
        (void)drv;
        ensure(true); // Skip
#endif
    }

    // Test GDALDataTypeUnion()
    template<> template<> void object::test<6>()
    {
        for(int i=GDT_Byte;i<GDT_TypeCount;i++)
        {
            for(int j=GDT_Byte;j<GDT_TypeCount;j++)
            {
                GDALDataType eDT1 = static_cast<GDALDataType>(i);
                GDALDataType eDT2 = static_cast<GDALDataType>(j);
                GDALDataType eDT = GDALDataTypeUnion(eDT1,eDT2 );
                ensure( eDT == GDALDataTypeUnion(eDT2,eDT1) );
                ensure( GDALGetDataTypeSize(eDT) >= GDALGetDataTypeSize(eDT1) );
                ensure( GDALGetDataTypeSize(eDT) >= GDALGetDataTypeSize(eDT2) );
                ensure( (GDALDataTypeIsComplex(eDT) && (GDALDataTypeIsComplex(eDT1) || GDALDataTypeIsComplex(eDT2))) ||
                        (!(GDALDataTypeIsComplex(eDT) && !GDALDataTypeIsComplex(eDT1) && !GDALDataTypeIsComplex(eDT2))) );
            }
        }
    }

    // Test GDALAdjustValueToDataType()
    template<> template<> void object::test<7>()
    {
        int bClamped, bRounded;

        ensure( GDALAdjustValueToDataType(GDT_Byte,255.0,NULL,NULL) == 255.0);
        ensure( GDALAdjustValueToDataType(GDT_Byte,255.0,&bClamped,&bRounded) == 255.0 && !bClamped && !bRounded);
        ensure( GDALAdjustValueToDataType(GDT_Byte,254.4,&bClamped,&bRounded) == 254.0 && !bClamped && bRounded);
        ensure( GDALAdjustValueToDataType(GDT_Byte,-1,&bClamped,&bRounded) == 0.0 && bClamped && !bRounded);
        ensure( GDALAdjustValueToDataType(GDT_Byte,256.0,&bClamped,&bRounded) == 255.0 && bClamped && !bRounded);

        ensure( GDALAdjustValueToDataType(GDT_UInt16,65535.0,&bClamped,&bRounded) == 65535.0 && !bClamped && !bRounded);
        ensure( GDALAdjustValueToDataType(GDT_UInt16,65534.4,&bClamped,&bRounded) == 65534.0 && !bClamped && bRounded);
        ensure( GDALAdjustValueToDataType(GDT_UInt16,-1,&bClamped,&bRounded) == 0.0 && bClamped && !bRounded);
        ensure( GDALAdjustValueToDataType(GDT_UInt16,65536.0,&bClamped,&bRounded) == 65535.0 && bClamped && !bRounded);

        ensure( GDALAdjustValueToDataType(GDT_Int16,-32768.0,&bClamped,&bRounded) == -32768.0 && !bClamped && !bRounded);
        ensure( GDALAdjustValueToDataType(GDT_Int16,32767.0,&bClamped,&bRounded) == 32767.0 && !bClamped && !bRounded);
        ensure( GDALAdjustValueToDataType(GDT_Int16,-32767.4,&bClamped,&bRounded) == -32767.0 && !bClamped && bRounded);
        ensure( GDALAdjustValueToDataType(GDT_Int16,32766.4,&bClamped,&bRounded) == 32766.0 && !bClamped && bRounded);
        ensure( GDALAdjustValueToDataType(GDT_Int16,-32769.0,&bClamped,&bRounded) == -32768.0 && bClamped && !bRounded);
        ensure( GDALAdjustValueToDataType(GDT_Int16,32768.0,&bClamped,&bRounded) == 32767.0 && bClamped && !bRounded);

        ensure( GDALAdjustValueToDataType(GDT_UInt32,10000000.0,&bClamped,&bRounded) == 10000000.0 && !bClamped && !bRounded);
        ensure( GDALAdjustValueToDataType(GDT_UInt32,10000000.4,&bClamped,&bRounded) == 10000000.0 && !bClamped && bRounded);
        ensure( GDALAdjustValueToDataType(GDT_UInt32,-1,&bClamped,&bRounded) == 0.0 && bClamped && !bRounded);

        ensure( GDALAdjustValueToDataType(GDT_Int32,-10000000.0,&bClamped,&bRounded) == -10000000.0 && !bClamped && !bRounded);
        ensure( GDALAdjustValueToDataType(GDT_Int32,10000000.0,&bClamped,&bRounded) == 10000000.0 && !bClamped && !bRounded);
        
        ensure( GDALAdjustValueToDataType(GDT_Float32, 0.0,&bClamped,&bRounded) == 0.0 && !bClamped && !bRounded);
        ensure( GDALAdjustValueToDataType(GDT_Float32, 1e-40,&bClamped,&bRounded) == 0.0 && !bClamped && !bRounded);
        ensure( GDALAdjustValueToDataType(GDT_Float32, 1.23,&bClamped,&bRounded) == static_cast<double>(1.23f) && !bClamped && !bRounded);
        ensure( GDALAdjustValueToDataType(GDT_Float32, -1e300,&bClamped,&bRounded) == -std::numeric_limits<float>::max() && bClamped && !bRounded);
        ensure( GDALAdjustValueToDataType(GDT_Float32, 1e300,&bClamped,&bRounded) == std::numeric_limits<float>::max() && bClamped && !bRounded);

        ensure( GDALAdjustValueToDataType(GDT_Float64, 0.0,&bClamped,&bRounded) == 0.0 && !bClamped && !bRounded);
        ensure( GDALAdjustValueToDataType(GDT_Float64, 1e-40,&bClamped,&bRounded) == 1e-40 && !bClamped && !bRounded);
        ensure( GDALAdjustValueToDataType(GDT_Float64, -1e40,&bClamped,&bRounded) == -1e40 && !bClamped && !bRounded);
        ensure( GDALAdjustValueToDataType(GDT_Float64, 1e40,&bClamped,&bRounded) == 1e40 && !bClamped && !bRounded);
    }

    class FakeBand: public GDALRasterBand
    {
        protected:
            virtual CPLErr IReadBlock(int, int, void*) { return CE_None; }
            virtual CPLErr IWriteBlock( int, int, void * ) { return CE_None; }

        public:
                    FakeBand(int nXSize, int nYSize) { nBlockXSize = nXSize;
                                                       nBlockYSize = nYSize; }
    };

    class DatasetWithErrorInFlushCache: public GDALDataset
    {
            bool bHasFlushCache;
        public:
            DatasetWithErrorInFlushCache() : bHasFlushCache(false) { }
           ~DatasetWithErrorInFlushCache() { FlushCache(); }
            virtual void FlushCache(void)
            {
                if( !bHasFlushCache)
                    CPLError(CE_Failure, CPLE_AppDefined, "some error");
                GDALDataset::FlushCache();
                bHasFlushCache = true;
            }
            virtual CPLErr SetProjection(const char*) { return CE_None; }
            virtual CPLErr SetGeoTransform(double*) { return CE_None; }

            static GDALDataset* CreateCopy(const char*, GDALDataset*,
                                    int, char **,
                                    GDALProgressFunc, 
                                    void *)
            {
                return new DatasetWithErrorInFlushCache();
            }

            static GDALDataset* Create(const char*, int nXSize, int nYSize, int, GDALDataType, char ** )
            {
                DatasetWithErrorInFlushCache* poDS = new DatasetWithErrorInFlushCache();
                poDS->eAccess = GA_Update;
                poDS->nRasterXSize = nXSize;
                poDS->nRasterYSize = nYSize;
                poDS->SetBand(1, new FakeBand(nXSize, nYSize));
                return poDS;
            }
    };

    // Test that GDALTranslate() detects error in flush cache
    template<> template<> void object::test<8>()
    {
        GDALDriver* poDriver = new GDALDriver();
        poDriver->SetDescription("DatasetWithErrorInFlushCache");
        poDriver->pfnCreateCopy = DatasetWithErrorInFlushCache::CreateCopy;
        GetGDALDriverManager()->RegisterDriver( poDriver );
        const char* args[] = { "-of", "DatasetWithErrorInFlushCache", NULL };
        GDALTranslateOptions* psOptions = GDALTranslateOptionsNew((char**)args, NULL);
        GDALDatasetH hSrcDS = GDALOpen("../gcore/data/byte.tif", GA_ReadOnly);
        CPLErrorReset();
        CPLPushErrorHandler(CPLQuietErrorHandler);
        GDALDatasetH hOutDS = GDALTranslate("", hSrcDS, psOptions, NULL);
        CPLPopErrorHandler();
        GDALClose(hSrcDS);
        GDALTranslateOptionsFree(psOptions);
        ensure(hOutDS == NULL);
        ensure(CPLGetLastErrorType() != CE_None);
        GetGDALDriverManager()->DeregisterDriver( poDriver );
        delete poDriver;
    }

    // Test that GDALWarp() detects error in flush cache
    template<> template<> void object::test<9>()
    {
        GDALDriver* poDriver = new GDALDriver();
        poDriver->SetDescription("DatasetWithErrorInFlushCache");
        poDriver->pfnCreate = DatasetWithErrorInFlushCache::Create;
        GetGDALDriverManager()->RegisterDriver( poDriver );
        const char* args[] = { "-of", "DatasetWithErrorInFlushCache", NULL };
        GDALWarpAppOptions* psOptions = GDALWarpAppOptionsNew((char**)args, NULL);
        GDALDatasetH hSrcDS = GDALOpen("../gcore/data/byte.tif", GA_ReadOnly);
        CPLErrorReset();
        CPLPushErrorHandler(CPLQuietErrorHandler);
        GDALDatasetH hOutDS = GDALWarp("/", NULL, 1, &hSrcDS, psOptions, NULL);
        CPLPopErrorHandler();
        GDALClose(hSrcDS);
        GDALWarpAppOptionsFree(psOptions);
        ensure(hOutDS == NULL);
        ensure(CPLGetLastErrorType() != CE_None);
        GetGDALDriverManager()->DeregisterDriver( poDriver );
        delete poDriver;
    }
} // namespace tut
