/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_package.hxx"
#include <com/sun/star/ucb/XSimpleFileAccess.hpp>
#include <com/sun/star/embed/ElementModes.hpp>
#include <com/sun/star/embed/StorageFormats.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/io/XSeekable.hpp>

#include <comphelper/storagehelper.hxx>

#include "xfactory.hxx"
#include "xstorage.hxx"


using namespace ::com::sun::star;

//-------------------------------------------------------------------------
sal_Bool CheckPackageSignature_Impl( const uno::Reference< io::XInputStream >& xInputStream,
									 const uno::Reference< io::XSeekable >& xSeekable )
{
	if ( !xInputStream.is() || !xSeekable.is() )
		throw uno::RuntimeException();

	if ( xSeekable->getLength() )
	{
		uno::Sequence< sal_Int8 > aData( 4 );
		xSeekable->seek( 0 );
		sal_Int32 nRead = xInputStream->readBytes( aData, 4 );
		xSeekable->seek( 0 );

		// TODO/LATER: should the disk spanned files be supported?
    	// 0x50, 0x4b, 0x07, 0x08
		return ( nRead == 4 && aData[0] == 0x50 && aData[1] == 0x4b && aData[2] == 0x03 && aData[3] == 0x04 );
	}
	else
		return sal_True; // allow to create a storage based on empty stream
}

//-------------------------------------------------------------------------
uno::Sequence< ::rtl::OUString > SAL_CALL OStorageFactory::impl_staticGetSupportedServiceNames()
{
    uno::Sequence< ::rtl::OUString > aRet(2);
    aRet[0] = ::rtl::OUString::createFromAscii("com.sun.star.embed.StorageFactory");
    aRet[1] = ::rtl::OUString::createFromAscii("com.sun.star.comp.embed.StorageFactory");
    return aRet;
}

//-------------------------------------------------------------------------
::rtl::OUString SAL_CALL OStorageFactory::impl_staticGetImplementationName()
{
    return ::rtl::OUString::createFromAscii("com.sun.star.comp.embed.StorageFactory");
}

//-------------------------------------------------------------------------
uno::Reference< uno::XInterface > SAL_CALL OStorageFactory::impl_staticCreateSelfInstance(
			const uno::Reference< lang::XMultiServiceFactory >& xServiceManager )
{
	return uno::Reference< uno::XInterface >( *new OStorageFactory( xServiceManager ) );
}

//-------------------------------------------------------------------------
uno::Reference< uno::XInterface > SAL_CALL OStorageFactory::createInstance()
	throw ( uno::Exception,
			uno::RuntimeException )
{
	// TODO: reimplement TempStream service to support XStream interface
	uno::Reference < io::XStream > xTempStream( 
						m_xFactory->createInstance( ::rtl::OUString::createFromAscii( "com.sun.star.io.TempFile" ) ),
						uno::UNO_QUERY );

	if ( !xTempStream.is() )
		throw uno::RuntimeException(); // TODO:

	return uno::Reference< uno::XInterface >( 
				static_cast< OWeakObject* >( new OStorage(	xTempStream,
															embed::ElementModes::READWRITE,
															uno::Sequence< beans::PropertyValue >(),
															m_xFactory,
															embed::StorageFormats::PACKAGE ) ),
				uno::UNO_QUERY );
}

//-------------------------------------------------------------------------
uno::Reference< uno::XInterface > SAL_CALL OStorageFactory::createInstanceWithArguments(
			const uno::Sequence< uno::Any >& aArguments )
	throw ( uno::Exception,
			uno::RuntimeException )
{
	// The request for storage can be done with up to three arguments

	// The first argument specifies a source for the storage
	// it can be URL, XStream, XInputStream.
	// The second value is a mode the storage should be open in.
	// And the third value is a media descriptor.

	sal_Int32 nArgNum = aArguments.getLength();
	OSL_ENSURE( nArgNum < 4, "Wrong parameter number" );

	if ( !nArgNum )
		return createInstance();

	// first try to retrieve storage open mode if any
	// by default the storage will be open in readonly mode
	sal_Int32 nStorageMode = embed::ElementModes::READ;
	if ( nArgNum >= 2 )
	{
		if( !( aArguments[1] >>= nStorageMode ) )
		{
			OSL_ENSURE( sal_False, "Wrong second argument!\n" );
			throw lang::IllegalArgumentException(); // TODO:
		}
		// it's allways possible to read written storage in this implementation
		nStorageMode |= embed::ElementModes::READ;
	}

	if ( ( nStorageMode & embed::ElementModes::TRUNCATE ) == embed::ElementModes::TRUNCATE
	  && ( nStorageMode & embed::ElementModes::WRITE ) != embed::ElementModes::WRITE )
		throw lang::IllegalArgumentException(); // TODO:

	// retrieve storage source stream
	::rtl::OUString aURL;
	uno::Reference< io::XStream > xStream;
	uno::Reference< io::XInputStream > xInputStream;

	if ( aArguments[0] >>= aURL )
	{
		if ( !aURL.getLength() )
		{
			OSL_ENSURE( sal_False, "Empty URL is provided!\n" );
			throw lang::IllegalArgumentException(); // TODO:
		}

		if ( aURL.equalsIgnoreAsciiCaseAsciiL( "vnd.sun.star.pkg", 16 ) )
		{
			OSL_ENSURE( sal_False, "Packages URL's are not valid for storages!\n" ); // ???
			throw lang::IllegalArgumentException(); // TODO:
		}

		uno::Reference < ::com::sun::star::ucb::XSimpleFileAccess > xTempAccess( 
				m_xFactory->createInstance ( 
						::rtl::OUString::createFromAscii( "com.sun.star.ucb.SimpleFileAccess" ) ),
				uno::UNO_QUERY );

		if ( !xTempAccess.is() )
			throw uno::RuntimeException(); // TODO:

		if ( nStorageMode & embed::ElementModes::WRITE )
			xStream = xTempAccess->openFileReadWrite( aURL );
		else
			xInputStream = xTempAccess->openFileRead( aURL );
	}
	else if ( !( aArguments[0] >>= xStream ) && !( aArguments[0] >>= xInputStream ) )
	{
		OSL_ENSURE( sal_False, "Wrong first argument!\n" );
		throw uno::Exception(); // TODO: Illegal argument
	}

	// retrieve mediadescriptor and set storage properties
	uno::Sequence< beans::PropertyValue > aDescr;
	uno::Sequence< beans::PropertyValue > aPropsToSet;
	
	sal_Int32 nStorageType = embed::StorageFormats::PACKAGE;
	
	if ( nArgNum >= 3 )
	{
		if( aArguments[2] >>= aDescr )
		{
			if ( aURL.getLength() )
			{
				aPropsToSet.realloc(1);
				aPropsToSet[0].Name = ::rtl::OUString::createFromAscii( "URL" );
				aPropsToSet[0].Value <<= aURL;
			}

			for ( sal_Int32 nInd = 0, nNumArgs = 1; nInd < aDescr.getLength(); nInd++ )
			{
				if ( aDescr[nInd].Name.equalsAscii( "InteractionHandler" )
				  || aDescr[nInd].Name.equalsAscii( "Password" )
				  || aDescr[nInd].Name.equalsAscii( "RepairPackage" )
				  || aDescr[nInd].Name.equalsAscii( "StatusIndicator" ) )
				  // || aDescr[nInd].Name.equalsAscii( "Unpacked" ) // TODO:
				{
					aPropsToSet.realloc( ++nNumArgs );
					aPropsToSet[nNumArgs-1].Name = aDescr[nInd].Name;
					aPropsToSet[nNumArgs-1].Value = aDescr[nInd].Value;
				}
				else if ( aDescr[nInd].Name.equalsAscii( "StorageFormat" ) )
				{
					::rtl::OUString aFormatName;
                    sal_Int32 nFormatID = 0;
                    if ( aDescr[nInd].Value >>= aFormatName )
                    {
                        if ( aFormatName.equals( PACKAGE_STORAGE_FORMAT_STRING ) )
                            nStorageType = embed::StorageFormats::PACKAGE;
                        else if ( aFormatName.equals( ZIP_STORAGE_FORMAT_STRING ) )
                            nStorageType = embed::StorageFormats::ZIP;
                        else if ( aFormatName.equals( OFOPXML_STORAGE_FORMAT_STRING ) )
                            nStorageType = embed::StorageFormats::OFOPXML;
                        else
                            throw lang::IllegalArgumentException( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( OSL_LOG_PREFIX ) ), uno::Reference< uno::XInterface >(), 1 );
                    }
                    else if ( aDescr[nInd].Value >>= nFormatID )
                    {
                        if ( nFormatID != embed::StorageFormats::PACKAGE
                          && nFormatID != embed::StorageFormats::ZIP
                          && nFormatID != embed::StorageFormats::OFOPXML )
                            throw lang::IllegalArgumentException( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( OSL_LOG_PREFIX ) ), uno::Reference< uno::XInterface >(), 1 );

                        nStorageType = nFormatID;
                    }
                    else
                        throw lang::IllegalArgumentException( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( OSL_LOG_PREFIX ) ), uno::Reference< uno::XInterface >(), 1 );
				}
				else
					OSL_ENSURE( sal_False, "Unacceptable property, will be ignored!\n" );
			}
		}
		else
		{
			OSL_ENSURE( sal_False, "Wrong third argument!\n" );
			throw uno::Exception(); // TODO: Illegal argument
		}

	}

	// create storage based on source
	if ( xInputStream.is() )
	{
		// if xInputStream is set the storage should be open from it
		if ( ( nStorageMode & embed::ElementModes::WRITE ) )
		  	throw uno::Exception(); // TODO: access denied

		uno::Reference< io::XSeekable > xSeekable( xInputStream, uno::UNO_QUERY );
		if ( !xSeekable.is() )
		{
			// TODO: wrap stream to let it be seekable
			OSL_ENSURE( sal_False, "Nonseekable streams are not supported for now!\n" );
		}

		if ( !CheckPackageSignature_Impl( xInputStream, xSeekable ) )
			throw io::IOException(); // TODO: this is not a package file

		return uno::Reference< uno::XInterface >( 
					static_cast< OWeakObject* >( new OStorage( xInputStream, nStorageMode, aPropsToSet, m_xFactory, nStorageType ) ),
					uno::UNO_QUERY );
	}
	else if ( xStream.is() )
	{
               if ( ( ( nStorageMode & embed::ElementModes::WRITE ) && !xStream->getOutputStream().is() )
		  || !xStream->getInputStream().is() )
		  	throw uno::Exception(); // TODO: access denied

		uno::Reference< io::XSeekable > xSeekable( xStream, uno::UNO_QUERY );
		if ( !xSeekable.is() )
		{
			// TODO: wrap stream to let it be seekable
			OSL_ENSURE( sal_False, "Nonseekable streams are not supported for now!\n" );
		}

		if ( !CheckPackageSignature_Impl( xStream->getInputStream(), xSeekable ) )
			throw io::IOException(); // TODO: this is not a package file

		return uno::Reference< uno::XInterface >( 
					static_cast< OWeakObject* >( new OStorage( xStream, nStorageMode, aPropsToSet, m_xFactory, nStorageType ) ),
					uno::UNO_QUERY );
	}

	throw uno::Exception(); // general error during creation
}

//-------------------------------------------------------------------------
::rtl::OUString SAL_CALL OStorageFactory::getImplementationName()
	throw ( uno::RuntimeException )
{
	return impl_staticGetImplementationName();
}

//-------------------------------------------------------------------------
sal_Bool SAL_CALL OStorageFactory::supportsService( const ::rtl::OUString& ServiceName )
	throw ( uno::RuntimeException )
{
	uno::Sequence< ::rtl::OUString > aSeq = impl_staticGetSupportedServiceNames();

	for ( sal_Int32 nInd = 0; nInd < aSeq.getLength(); nInd++ )
    	if ( ServiceName.compareTo( aSeq[nInd] ) == 0 )
        	return sal_True;

	return sal_False;
}

//-------------------------------------------------------------------------
uno::Sequence< ::rtl::OUString > SAL_CALL OStorageFactory::getSupportedServiceNames()
	throw ( uno::RuntimeException )
{
	return impl_staticGetSupportedServiceNames();
}

