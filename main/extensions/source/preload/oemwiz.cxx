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
#include "precompiled_extensions.hxx"
#include "oemwiz.hxx"
#include "componentmodule.hxx"
#include <tools/debug.hxx>
#include <sfx2/docfile.hxx>
#include <svl/itemset.hxx>
#include <svl/itempool.hxx>
#include <vcl/msgbox.hxx>
#include <unotools/pathoptions.hxx>
#ifndef _EXTENSIONS_PRELOAD_PRELOAD_HRC_
#include "preload.hrc"
#endif
#ifndef _SVX_SVXIDS_HRC
#include <svx/svxids.hrc>
#endif
#include <sfx2/app.hxx>
#include <svtools/xtextedt.hxx>
#include "osl/diagnose.h"
#include "tools/urlobj.hxx"

#include <sfx2/sfxdlg.hxx>
#include <sfx2/tabdlg.hxx>
#include <svx/dialogs.hrc>

//.........................................................................
namespace preload
{
//.........................................................................

	using namespace ::com::sun::star::uno;
	using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::beans;
//.........................................................................

    struct OEMPreloadDialog_Impl
    {
        SfxItemSet*     pSet;
        TabPage* pWelcomePage;
        TabPage* pLicensePage;
        TabPage* pUserDataPage;

        OEMPreloadDialog_Impl(OEMPreloadDialog* pDialog);
        ~OEMPreloadDialog_Impl()
        {
            delete pWelcomePage;
            delete pLicensePage;
            delete pUserDataPage;
            delete pSet;
        }
        void WriteUserData();
    };
/* -----------------------------14.11.2001 11:33------------------------------

 ---------------------------------------------------------------------------*/
    OEMPreloadDialog_Impl::OEMPreloadDialog_Impl(OEMPreloadDialog* pDialog)
        {
            SfxItemPool& rPool = SFX_APP()->GetPool();
            pSet = new SfxItemSet(rPool, SID_FIELD_GRABFOCUS, SID_FIELD_GRABFOCUS);
			SfxAbstractDialogFactory* pFact = SfxAbstractDialogFactory::Create();
			if ( pFact )
			{
				CreateTabPage pFunc = pFact->GetTabPageCreatorFunc(RID_SFXPAGE_GENERAL);
            	pUserDataPage = (*pFunc)(pDialog, *pSet);
            	((SfxTabPage*)pUserDataPage)->Reset(*pSet);
			}
			else
				pUserDataPage = NULL;
            pWelcomePage = new OEMWelcomeTabPage(pDialog);
            pLicensePage = new OEMLicenseTabPage(pDialog);
        }
/* -----------------------------14.11.2001 11:33------------------------------

 ---------------------------------------------------------------------------*/
        void OEMPreloadDialog_Impl::WriteUserData()
        {
			if ( pUserDataPage )
            	((SfxTabPage*)pUserDataPage)->FillItemSet(*pSet);
        }

/* -----------------------------14.11.2001 11:33------------------------------

 ---------------------------------------------------------------------------*/
    //=====================================================================
    //= OEMPreloadDialog
	//=====================================================================
	//---------------------------------------------------------------------
    OEMPreloadDialog::OEMPreloadDialog( Window* _pParent,
			const Reference< XPropertySet >& /*_rxObjectModel*/, const Reference< XMultiServiceFactory >& /*_rxORB*/ )
        :WizardDialog(_pParent, ModuleRes(RID_DLG_OEMWIZARD)/*, _rxObjectModel, _rxORB*/)
        ,aPrevPB(this,      ModuleRes(PB_PREV   ))
        ,aNextPB(this,      ModuleRes(PB_NEXT   ))
        ,aCancelPB(this,    ModuleRes(PB_CANCEL ))
        ,aAcceptST(ModuleRes(ST_ACCEPT))
        ,aFinishST(ModuleRes(ST_FINISH))
        ,aLicense(ModuleRes(ST_LICENSE_AGREEMENT))
        ,aUserData(ModuleRes(ST_INSERT_USER_DATA))
        ,pImpl(new OEMPreloadDialog_Impl(this))
	{
          FreeResource();
          aDlgTitle = GetText();
          aPrevPB.Enable(sal_False);
          aNextST = aNextPB.GetText();
          aPrevPB.SetClickHdl(LINK(this, OEMPreloadDialog, NextPrevPageHdl));
          aNextPB.SetClickHdl(LINK(this, OEMPreloadDialog, NextPrevPageHdl));
          AddButton( &aPrevPB, WIZARDDIALOG_BUTTON_STDOFFSET_X );
          AddButton( &aNextPB, WIZARDDIALOG_BUTTON_STDOFFSET_X );
          AddButton( &aCancelPB, WIZARDDIALOG_BUTTON_STDOFFSET_X );

          SetPrevButton(&aPrevPB);
          SetNextButton(&aNextPB);

          AddPage( pImpl->pWelcomePage );
          AddPage( pImpl->pLicensePage );
		  if ( pImpl->pUserDataPage )
          	AddPage( pImpl->pUserDataPage );
          SetPage( OEM_WELCOME, pImpl->pWelcomePage );
          SetPage( OEM_LICENSE, pImpl->pLicensePage );
		  if ( pImpl->pUserDataPage )
          	SetPage( OEM_USERDATA, pImpl->pUserDataPage );
          ShowPage( OEM_WELCOME );
    }
/* -----------------------------14.11.2001 11:33------------------------------

 ---------------------------------------------------------------------------*/
    OEMPreloadDialog::~OEMPreloadDialog( )
    {
        delete pImpl;
    }
/* -----------------------------14.11.2001 11:33------------------------------

 ---------------------------------------------------------------------------*/
    IMPL_LINK(OEMPreloadDialog, NextPrevPageHdl, PushButton*, pButton)
    {
        if(pButton == &aPrevPB)
        {
            if(GetCurLevel())
                ShowPage(GetCurLevel() - 1);
        }
           else if(OEM_USERDATA > GetCurLevel())
            ShowPage(GetCurLevel() + 1);
		else
		{
            pImpl->WriteUserData();
            Finnish(RET_OK);
		}

        String sTitle(aDlgTitle);

        switch(GetCurLevel())
        {
            case OEM_WELCOME:
                aNextPB.SetText(aNextST);
                aNextPB.Enable(sal_True);
            break;
            case OEM_LICENSE:
                sTitle += aLicense;
                aNextPB.SetText(aNextST);
                aCancelPB.GrabFocus();
            break;
            case OEM_USERDATA:
                sTitle += aUserData;
                aNextPB.SetText(aFinishST);
            break;
        }
        SetText(sTitle);
        aPrevPB.Enable(GetCurLevel() != OEM_WELCOME);
        return 0;
    }
/* -----------------------------14.11.2001 11:33------------------------------

 ---------------------------------------------------------------------------*/
    sal_Bool OEMPreloadDialog::LoadFromLocalFile(const String& rFileName, String& rContent)
    {
        SvtPathOptions aPathOpt;
        String sFileName = aPathOpt.GetUserConfigPath();//GetModulePath();
        INetURLObject aURLObject(sFileName);
        OSL_ASSERT(aURLObject.getSegmentCount() >= 2);
        aURLObject.removeSegment(); //remove '/config'
        aURLObject.removeSegment(); //remove '/user'
        sFileName = aURLObject.GetMainURL(INetURLObject::DECODE_TO_IURI);
        sFileName += rFileName;

        SfxMedium aMedium( sFileName,STREAM_READ, sal_True );
        SvStream* pInStream = aMedium.GetInStream();
        if( !pInStream )
            return sal_False;

        pInStream->ReadCString( rContent, RTL_TEXTENCODING_UTF8 );

        xub_StrLen nPos;
        while ( ( nPos = rContent.Search( 12 )) != STRING_NOTFOUND )
            rContent.Erase( nPos, 1 );
        return sal_True;
    }


    void OEMPreloadDialog::SetCancelString( const String& rText )
    {
        aCancelPB.SetText(rText);
    }

/* -----------------------------13.11.2001 12:29------------------------------

 ---------------------------------------------------------------------------*/
    OEMWelcomeTabPage::OEMWelcomeTabPage(Window* pParent) :
        TabPage(pParent, ModuleRes(RID_TP_WELCOME)),
        aInfoFT(this, ModuleRes(FT_INFO))
    {
        FreeResource();
    }
/* -----------------------------13.11.2001 12:30------------------------------

 ---------------------------------------------------------------------------*/
    OEMWelcomeTabPage::~OEMWelcomeTabPage()
    {
    }
/* -----------------------------13.11.2001 12:29------------------------------

 ---------------------------------------------------------------------------*/
    OEMLicenseTabPage::OEMLicenseTabPage(OEMPreloadDialog* pParent) :
        TabPage(pParent, ModuleRes(RID_TP_LICENSE)),
        aLicenseML(this, ModuleRes(ML_LICENSE)),
        aInfo1FT(this, ModuleRes(FT_INFO1)),
        aInfo2FT(this, ModuleRes(FT_INFO2)),
        aInfo3FT(this, ModuleRes(FT_INFO3)),
        aInfo2_1FT(this, ModuleRes(FT_INFO2_1)),
        aInfo3_1FT(this, ModuleRes(FT_INFO3_1)),
        aCBAccept(this, ModuleRes(CB_ACCEPT)),
        aPBPageDown(this, ModuleRes(PB_PAGEDOWN)),
        aArrow(this, ModuleRes(IMG_ARROW)),
        aStrAccept( ModuleRes(LICENCE_ACCEPT) ),
	    aStrNotAccept( ModuleRes(LICENCE_NOTACCEPT) ),
        bEndReached(sal_False),
        pPreloadDialog(pParent)
    {
        FreeResource();

        aLicenseML.SetEndReachedHdl( LINK(this, OEMLicenseTabPage, EndReachedHdl) );
        aLicenseML.SetScrolledHdl( LINK(this, OEMLicenseTabPage, ScrolledHdl) );

        aPBPageDown.SetClickHdl( LINK(this, OEMLicenseTabPage, PageDownHdl) );
        aCBAccept.SetClickHdl( LINK(this, OEMLicenseTabPage, AcceptHdl) );

        // We want a automatic repeating page down button
        WinBits aStyle = aPBPageDown.GetStyle();
        aStyle |= WB_REPEAT;
        aPBPageDown.SetStyle( aStyle );

        aOldCancelText = pPreloadDialog->GetCancelString();
        pPreloadDialog->SetCancelString( aStrNotAccept );

        String aText = aInfo2FT.GetText();
        aText.SearchAndReplaceAll( UniString::CreateFromAscii("%PAGEDOWN"), aPBPageDown.GetText() );
        aInfo2FT.SetText( aText );
    }
/* -----------------------------13.11.2001 12:30------------------------------

 ---------------------------------------------------------------------------*/
    OEMLicenseTabPage::~OEMLicenseTabPage()
    {
    }
/* ---------------------------14.11.2001 11:24----------------------------

    -----------------------------------------------------------------------*/
    void OEMLicenseTabPage::ActivatePage()
    {
        if(!aLicenseML.GetText().Len())
        {
        aLicenseML.SetLeftMargin( 5 );
        String sLicense;
#ifdef UNX
        OEMPreloadDialog::LoadFromLocalFile(String::CreateFromAscii("LICENSE"), sLicense);
#else
        OEMPreloadDialog::LoadFromLocalFile(String::CreateFromAscii("license.txt"), sLicense);
#endif
        aLicenseML.SetText( sLicense );
        }

        EnableControls();
    }

    //------------------------------------------------------------------------
    IMPL_LINK( OEMLicenseTabPage, AcceptHdl, CheckBox *, EMPTYARG )
    {
        EnableControls();
        return 0;
    }

    //------------------------------------------------------------------------
    IMPL_LINK( OEMLicenseTabPage, PageDownHdl, PushButton *, EMPTYARG )
    {
        aLicenseML.ScrollDown( SCROLL_PAGEDOWN );
        return 0;
    }

    //------------------------------------------------------------------------
    IMPL_LINK( OEMLicenseTabPage, EndReachedHdl, LicenceView *, EMPTYARG )
    {
        bEndReached = sal_True;

        EnableControls();
        aCBAccept.GrabFocus();

        return 0;
    }

    //------------------------------------------------------------------------
    IMPL_LINK( OEMLicenseTabPage, ScrolledHdl, LicenceView *, EMPTYARG )
    {
        EnableControls();

        return 0;
    }

    //------------------------------------------------------------------------
    void OEMLicenseTabPage::EnableControls()
    {
        if( !bEndReached &&
            ( aLicenseML.IsEndReached() || !aLicenseML.GetText().Len() ) )
            bEndReached = sal_True;

        if ( bEndReached )
        {
            Point aPos( 0, aInfo3_1FT.GetPosPixel().Y() );
            aArrow.SetPosPixel( aPos );
            aCBAccept.Enable();
        }
        else
        {
            Point aPos( 0, aInfo2_1FT.GetPosPixel().Y() );
            aArrow.SetPosPixel( aPos );
            aCBAccept.Disable();
        }

        if ( aLicenseML.IsEndReached() )
            aPBPageDown.Disable();
        else
            aPBPageDown.Enable();

        if ( aCBAccept.IsChecked() )
        {
            PushButton *pNext = pPreloadDialog->GetNextButton();
            if ( ! pNext->IsEnabled() )
            {
                pPreloadDialog->SetCancelString( aOldCancelText );
                pNext->Enable(sal_True);
            }
        }
        else
        {
            PushButton *pNext = pPreloadDialog->GetNextButton();
            if ( pNext->IsEnabled() )
            {
                pPreloadDialog->SetCancelString( aStrNotAccept );
                pNext->Enable(sal_False);
            }
        }
    }

    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    LicenceView::LicenceView( Window* pParent, const ResId& rResId )
        : MultiLineEdit( pParent, rResId )
    {
        SetLeftMargin( 5 );

        mbEndReached = IsEndReached();

	    StartListening( *GetTextEngine() );
    }

    //------------------------------------------------------------------------
    LicenceView::~LicenceView()
    {
        maEndReachedHdl = Link();
        maScrolledHdl   = Link();

        EndListeningAll();
    }

    //------------------------------------------------------------------------
    void LicenceView::ScrollDown( ScrollType eScroll )
    {
        ScrollBar*  pScroll = GetVScrollBar();

        if ( pScroll )
            pScroll->DoScrollAction( eScroll );
    }

    //------------------------------------------------------------------------
    sal_Bool LicenceView::IsEndReached() const
    {
        sal_Bool bEndReached;

        ExtTextView*    pView = GetTextView();
        ExtTextEngine*  pEdit = GetTextEngine();
        sal_uLong           nHeight = pEdit->GetTextHeight();
        Size            aOutSize = pView->GetWindow()->GetOutputSizePixel();
        Point           aBottom( 0, aOutSize.Height() );

        if ( (sal_uLong) pView->GetDocPos( aBottom ).Y() >= nHeight - 1 )
            bEndReached = sal_True;
        else
            bEndReached = sal_False;

        return bEndReached;
    }

    //------------------------------------------------------------------------
    void LicenceView::Notify( SfxBroadcaster& /*rBC*/, const SfxHint& rHint )
    {
        if ( rHint.IsA( TYPE(TextHint) ) )
        {
            sal_Bool    bLastVal = EndReached();
            sal_uLong   nId = ((const TextHint&)rHint).GetId();

            if ( nId == TEXT_HINT_PARAINSERTED )
            {
                if ( bLastVal )
                    mbEndReached = IsEndReached();
            }
            else if ( nId == TEXT_HINT_VIEWSCROLLED )
            {
                if ( ! mbEndReached )
                    mbEndReached = IsEndReached();
                maScrolledHdl.Call( this );
            }

            if ( EndReached() && !bLastVal )
            {
                maEndReachedHdl.Call( this );
            }
        }
    }

    //------------------------------------------------------------------------

//.........................................................................
}   // namespace preload
//.........................................................................


