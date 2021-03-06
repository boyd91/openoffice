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
#include "precompiled_dbaccess.hxx"

#ifndef DBAUI_DATAVIEW_HXX
#include "dataview.hxx"
#endif
#ifndef _TOOLKIT_HELPER_VCLUNOHELPER_HXX_ 
#include <toolkit/unohlp.hxx>
#endif
#ifndef _COMPHELPER_TYPES_HXX_
#include <comphelper/types.hxx>
#endif
#include <comphelper/namedvaluecollection.hxx>
#ifndef _SFXAPP_HXX //autogen wg. SFX_APP
#include <sfx2/app.hxx>
#endif
#ifndef _SFXIMGMGR_HXX 
#include <sfx2/imgmgr.hxx>
#endif
#ifndef DBAUI_ICONTROLLER_HXX
#include "IController.hxx"
#endif
#ifndef DBAUI_TOOLS_HXX
#include "UITools.hxx"
#endif
#ifndef _SFX_HRC
#include <sfx2/sfx.hrc>
#endif
#ifndef _SVTOOLS_IMGDEF_HXX
#include <svtools/imgdef.hxx>
#endif
#include <tools/diagnose_ex.h>

//.........................................................................
namespace dbaui
{
//.........................................................................
	using namespace ::com::sun::star::uno;
	using namespace ::com::sun::star::beans;
	using namespace ::com::sun::star::util;
	using namespace ::com::sun::star::lang;
	using namespace ::com::sun::star::frame;

	//=====================================================================
	//= ColorChanger
	//=====================================================================
	class ColorChanger
	{
	protected:
		OutputDevice*	m_pDev;

	public:
		ColorChanger( OutputDevice* _pDev, const Color& _rNewLineColor, const Color& _rNewFillColor )
			:m_pDev( _pDev )
		{
			m_pDev->Push( PUSH_LINECOLOR | PUSH_FILLCOLOR );
			m_pDev->SetLineColor( _rNewLineColor );
			m_pDev->SetFillColor( _rNewFillColor );
		}

		~ColorChanger()
		{
			m_pDev->Pop();
		}
	};

	DBG_NAME(ODataView)
	// -------------------------------------------------------------------------
	ODataView::ODataView(	Window* pParent, 
							IController& _rController,
							const Reference< XMultiServiceFactory >& _rFactory, 
							WinBits nStyle)
		:Window(pParent,nStyle)
		,m_xServiceFactory(_rFactory)
		,m_rController( _rController )
		,m_aSeparator( this )
	{
		DBG_CTOR(ODataView,NULL);
		m_rController.acquire();
		m_pAccel.reset(::svt::AcceleratorExecute::createAcceleratorHelper());
        m_aSeparator.Show();
	}

	// -------------------------------------------------------------------------
	void ODataView::Construct()
	{
	}

	// -------------------------------------------------------------------------
	ODataView::~ODataView()
	{
		DBG_DTOR(ODataView,NULL);

		m_rController.release();		
	}

	// -------------------------------------------------------------------------
	void ODataView::resizeDocumentView( Rectangle& /*_rPlayground*/ )
	{
	}

	// -------------------------------------------------------------------------
	void ODataView::Paint( const Rectangle& _rRect )
	{
		//.................................................................
		// draw the background
		{
			ColorChanger aColors( this, COL_TRANSPARENT, GetSettings().GetStyleSettings().GetFaceColor() );
			DrawRect( _rRect );
		}

		// let the base class do anything it needs
		Window::Paint( _rRect );
	}

	// -------------------------------------------------------------------------
	void ODataView::resizeAll( const Rectangle& _rPlayground )
	{
		Rectangle aPlayground( _rPlayground );

		// position the separator
		const Size aSeparatorSize = Size( aPlayground.GetWidth(), 2 );
		m_aSeparator.SetPosSizePixel( aPlayground.TopLeft(), aSeparatorSize );
		aPlayground.Top() += aSeparatorSize.Height() + 1;

		// position the controls of the document's view
		resizeDocumentView( aPlayground );
	}

	// -------------------------------------------------------------------------
	void ODataView::Resize()
	{
		Window::Resize();
		resizeAll( Rectangle( Point( 0, 0), GetSizePixel() ) );
	}
	// -----------------------------------------------------------------------------
	long ODataView::PreNotify( NotifyEvent& _rNEvt )
	{
		bool bHandled = false;
		switch ( _rNEvt.GetType() )
		{
			case EVENT_KEYINPUT:
			{
				const KeyEvent* pKeyEvent =	_rNEvt.GetKeyEvent();
				const KeyCode& aKeyCode = pKeyEvent->GetKeyCode();
				if ( m_pAccel.get() && m_pAccel->execute( aKeyCode ) )
                    // the accelerator consumed the event
                    return 1L;
			}
            // NO break
            case EVENT_KEYUP:
            case EVENT_MOUSEBUTTONDOWN:
            case EVENT_MOUSEBUTTONUP:
                bHandled = m_rController.interceptUserInput( _rNEvt );
                break;
		}
		return bHandled ? 1L : Window::PreNotify( _rNEvt );
	}
	// -----------------------------------------------------------------------------
	void ODataView::StateChanged( StateChangedType nType )
	{
		Window::StateChanged( nType );

		if ( nType == STATE_CHANGE_CONTROLBACKGROUND )
		{
			// Check if we need to get new images for normal/high contrast mode
			m_rController.notifyHiContrastChanged();
		}

        if ( nType == STATE_CHANGE_INITSHOW )
        {
            // now that there's a view which is finally visible, remove the "Hidden" value from the
            // model's arguments.
            try
            {
                Reference< XController > xController( m_rController.getXController(), UNO_SET_THROW );
                Reference< XModel > xModel( xController->getModel(), UNO_QUERY );
                if ( xModel.is() )
                {
                    ::comphelper::NamedValueCollection aArgs( xModel->getArgs() );
                    aArgs.remove( "Hidden" );
                    xModel->attachResource( xModel->getURL(), aArgs.getPropertyValues() );
                }
            }
            catch( const Exception& )
            {
            	DBG_UNHANDLED_EXCEPTION();
            }
        }
	}
	// -----------------------------------------------------------------------------
	void ODataView::DataChanged( const DataChangedEvent& rDCEvt )
	{
		Window::DataChanged( rDCEvt );

		if ( (rDCEvt.GetType() == DATACHANGED_FONTS) ||
			(rDCEvt.GetType() == DATACHANGED_DISPLAY) ||
			(rDCEvt.GetType() == DATACHANGED_FONTSUBSTITUTION) ||
			((rDCEvt.GetType() == DATACHANGED_SETTINGS) &&
			(rDCEvt.GetFlags() & SETTINGS_STYLE)) )
		{
			// Check if we need to get new images for normal/high contrast mode
			m_rController.notifyHiContrastChanged();
		}
	}
	// -----------------------------------------------------------------------------
	void ODataView::attachFrame(const Reference< XFrame >& _xFrame)
	{
		m_pAccel->init(m_xServiceFactory,_xFrame);
	}
//.........................................................................
}
// namespace dbaui
//.........................................................................
