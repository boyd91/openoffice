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


#ifndef _CREATEADDRESSLISTDIALOG_HXX
#define _CREATEADDRESSLISTDIALOG_HXX

#include <sfx2/basedlgs.hxx>

#ifndef _SV_BUTTON_HXX
#include <vcl/button.hxx>
#endif
#include <svtools/stdctrl.hxx>
#include <vcl/field.hxx>
#include <vcl/lstbox.hxx>
#include <vector>

class SwAddressControl_Impl;
class SwMailMergeConfigItem;
/*-- 19.04.2004 12:09:46---------------------------------------------------
    container of the created database
  -----------------------------------------------------------------------*/
struct SwCSVData
{
    ::std::vector< ::rtl::OUString >                    aDBColumnHeaders;
    ::std::vector< ::std::vector< ::rtl::OUString> >     aDBData;
};
/*-- 08.04.2004 14:04:39---------------------------------------------------

  -----------------------------------------------------------------------*/
class SwFindEntryDialog;
class SwCreateAddressListDialog : public SfxModalDialog
{
    FixedInfo               m_aAddressInformation;
    SwAddressControl_Impl*  m_pAddressControl;

    PushButton              m_aNewPB;
    PushButton              m_aDeletePB;
    PushButton              m_aFindPB;
    PushButton              m_aCustomizePB;

    FixedInfo               m_aViewEntriesFI;
    PushButton              m_aStartPB;
    PushButton              m_aPrevPB;
    NumericField            m_aSetNoNF;
    PushButton              m_aNextPB;
    PushButton              m_aEndPB;

    FixedLine               m_aSeparatorFL;

    OKButton                m_aOK;
    CancelButton            m_aCancel;
    HelpButton              m_aHelp;

    String                  m_sAddressListFilterName;
    String                  m_sURL;

    SwCSVData*              m_pCSVData;
    SwFindEntryDialog*      m_pFindDlg;

    DECL_LINK(NewHdl_Impl, PushButton*);
    DECL_LINK(DeleteHdl_Impl, PushButton*);
    DECL_LINK(FindHdl_Impl, PushButton*);
    DECL_LINK(CustomizeHdl_Impl, PushButton*);
    DECL_LINK(OkHdl_Impl, PushButton*);
    DECL_LINK(DBCursorHdl_Impl, PushButton*);
    DECL_LINK(DBNumCursorHdl_Impl, NumericField*);

    void UpdateButtons();

public:
    SwCreateAddressListDialog(
            Window* pParent, const String& rURL, SwMailMergeConfigItem& rConfig);
    ~SwCreateAddressListDialog();

    const String&           GetURL() const {    return m_sURL;    }
    void                    Find( const String& rSearch, sal_Int32 nColumn);
};
/*-- 13.04.2004 13:30:21---------------------------------------------------

  -----------------------------------------------------------------------*/
class SwFindEntryDialog : public ModelessDialog
{
    FixedText               m_aFindFT;
    Edit                    m_aFindED;
    CheckBox                m_aFindOnlyCB;
    ListBox                 m_aFindOnlyLB;

    PushButton              m_aFindPB;
    CancelButton            m_aCancel;
    HelpButton              m_aHelp;

    SwCreateAddressListDialog*  m_pParent;

    DECL_LINK(FindHdl_Impl, PushButton*);
    DECL_LINK(FindEnableHdl_Impl, Edit*);
    DECL_LINK(CloseHdl_Impl, PushButton*);

public:
    SwFindEntryDialog(SwCreateAddressListDialog* pParent);
    ~SwFindEntryDialog();

    ListBox&                GetFieldsListBox(){return m_aFindOnlyLB;}
    String                  GetFindString() const {return m_aFindED.GetText();}
};

#endif

