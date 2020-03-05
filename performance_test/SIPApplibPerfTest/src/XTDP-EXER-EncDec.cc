/******************************************************************************
 * Copyright (c) 2000-2020 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               XTDP-EXER-EncDec.cc
//  Description:        XTDP EXER encoder/decoder
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 494
//  Updated:            2008.12.17
//  Contact:            http://ttcn.ericsson.se
//



#include "XTDPasp_Types.hh"
#include "XTDP_PDU_Defs.hh"
#include <memory.h>
#include "XUL_XTDL.hh"

#define XML_TAG_WINDOW          "window"
#define XML_TAG_TABBOX          "tabbox"
#define XML_TAG_TABS            "tabs"
#define XML_TAG_TAB             "tab"
#define XML_TAG_TABPANELS       "tabpanels"
#define XML_TAG_TABPANEL        "tabpanel"
#define XML_TAG_HBOX            "hbox"
#define XML_TAG_LABEL           "label"
#define XML_TAG_SPACER          "spacer"
#define XML_TAG_BUTTON          "button"
#define XML_TAG_TEXTBOX         "textbox"
#define XML_TAG_CHART           "chart"
#define XML_TAG_TRACE           "trace"
#define XML_TAG_TOOLBAR         "toolbar"
#define XML_TAG_TOOLBARBUTTON   "toolbarbutton"
#define XML_TAG_SEPARATOR       "separator"
#define XML_TAG_IMAGE           "image"
#define XML_TAG_TREE            "tree"
#define XML_TAG_TREECOLS        "treecols"
#define XML_TAG_TREECOL         "treecol"
#define XML_TAG_TREECHILDREN    "treechildren"
#define XML_TAG_TREEITEM        "treeitem"
#define XML_TAG_TREEROW         "treerow"
#define XML_TAG_TREECELL        "treecell"
#define XML_TAG_LISTBOX         "listbox"
#define XML_TAG_LISTITEM        "listitem"
#define XML_TAG_NUMERICALWIDGET "numericalwidget"
#define XML_TAG_MENULIST        "menulist"
#define XML_TAG_MENUPOPUP       "menupopup"
#define XML_TAG_MENUITEM        "menuitem"
#define XML_TAG_DISTRIBUTIONCHART  "distributionchart"
#define XML_TAG_INTERVALLIMITS  "intervallimits"
#define XML_TAG_VALUELIST       "valuelist"


extern XTDP__PDU__Defs::XTDP__Message parse_xtdp(const char* xtdp_str);
extern XTDP__PDU__Defs::XTDP__XML__Tag parse_xul(const char* xul_str);

namespace XTDPasp__Types {

//XUL encoding functions
  expstring_t enc_XMLTag_Widgets(expstring_t xtdp_str, const XUL__XTDL::WidgetList& widgets);
  expstring_t enc_XMLTag_Window(expstring_t xtdp_str, const XUL__XTDL::Window& window);
  expstring_t enc_XMLTag_Tabbox(expstring_t xtdp_str, const XUL__XTDL::Tabbox& tabbox);
  expstring_t enc_XMLTag_Tabs(expstring_t xtdp_str, const XUL__XTDL::Tabs& tabs);
  expstring_t enc_XMLTag_Tab(expstring_t xtdp_str, const XUL__XTDL::Tab& tab);
  expstring_t enc_XMLTag_TabPanels(expstring_t xtdp_str, const XUL__XTDL::Tabpanels& tabpanels);
  expstring_t enc_XMLTag_Tabpanel(expstring_t xtdp_str, const XUL__XTDL::Tabpanel& tabpanel);
  expstring_t enc_XMLTag_Tree(expstring_t xtdp_str, const XUL__XTDL::Tree& tree);
  expstring_t enc_XMLTag_TreeCols(expstring_t xtdp_str, const XUL__XTDL::Treecols& treecols);
  expstring_t enc_XMLTag_TreeCol(expstring_t xtdp_str, const XUL__XTDL::Treecol& treecol);
  expstring_t enc_XMLTag_TreeChildren(expstring_t xtdp_str, const XUL__XTDL::Treechildren& treechildren);
  expstring_t enc_XMLTag_TreeItem(expstring_t xtdp_str, const XUL__XTDL::Treeitem& treeitem);
  expstring_t enc_XMLTag_TreeRow(expstring_t xtdp_str, const XUL__XTDL::Treerow& treerow);
  expstring_t enc_XMLTag_TreeCell(expstring_t xtdp_str, const XUL__XTDL::Treecell& treecell);
  expstring_t enc_XMLTag_HBox(expstring_t xtdp_str, const XUL__XTDL::Hbox& hbox);
  expstring_t enc_XMLTag_Label(expstring_t xtdp_str, const XUL__XTDL::Label& label);
  expstring_t enc_XMLTag_Spacer(expstring_t xtdp_str, const XUL__XTDL::Spacer& spacer);
  expstring_t enc_XMLTag_Button(expstring_t xtdp_str, const XUL__XTDL::Button& button);
  expstring_t enc_XMLTag_Textbox(expstring_t xtdp_str, const XUL__XTDL::Textbox& textbox);
  expstring_t enc_XMLTag_Chart(expstring_t xtdp_str, const XUL__XTDL::Chart& chart);
  expstring_t enc_XMLTag_Trace(expstring_t xtdp_str, const XUL__XTDL::Trace& trace);
  expstring_t enc_XMLTag_Toolbar(expstring_t xtdp_str, const XUL__XTDL::Toolbar& toolbar);
  expstring_t enc_XMLTag_Toolbarbutton(expstring_t xtdp_str, const XUL__XTDL::Toolbarbutton& toolbarbutton);
  expstring_t enc_XMLTag_Image(expstring_t xtdp_str, const XUL__XTDL::Image& image);
  expstring_t enc_XMLTag_Separator(expstring_t xtdp_str, const XUL__XTDL::Separator& separator);
  expstring_t enc_XMLTag_Listbox(expstring_t xtdp_str, const XUL__XTDL::Listbox& listbox);
  expstring_t enc_XMLTag_Listitem(expstring_t xtdp_str, const XUL__XTDL::Listitem& listitem);
  expstring_t enc_XMLTag_Numericalwidget(expstring_t xtdp_str, const XUL__XTDL::Numericalwidget& numericalwidget);
  expstring_t enc_XMLTag_Menulist(expstring_t xtdp_str, const XUL__XTDL::Menulist& menulist);
  expstring_t enc_XMLTag_Menuitem(expstring_t xtdp_str, const XUL__XTDL::Menuitem& menuitem);
  expstring_t enc_XMLTag_Distributionchart(expstring_t xtdp_str, const XUL__XTDL::Distributionchart& distributionchart);
  expstring_t enc_XMLTag_Valuelist(expstring_t xtdp_str, const XUL__XTDL::DistributionchartValuelist& valuelist);

  /* checks whether a univ.charstring contains space*/
  bool isCorrectId(const UNIVERSAL_CHARSTRING& uc)
  {
	  for (int i=0; i<uc.lengthof(); ++i)
	  {
		  if (((char)unichar2int(uc[i].get_uchar())) == ' ')
		  {
			  return false;
		  }
	  }
	  return true;
  }

  /* escape special XML characters, only for content */
  expstring_t mputCescaped(expstring_t str, char c)
  {
	  switch (c)
	  {
	  case '<':
		  str = mputstr(str, "&lt;");
		  break;
	  case '>':
		  str = mputstr(str, "&gt;");
		  break;
	  case '&':
		  str = mputstr(str, "&amp;");
		  break;
	  case '\'':
		  str = mputstr(str, "&apos;");
		  break;
	  case '"':
		  str = mputstr(str, "&quot;");
		  break;
	  default:
		  str = mputc(str, c);
	  }
	  return str;
  }

  expstring_t mputUC(expstring_t str, const UNIVERSAL_CHARSTRING& uc)
  {
	  int uc_len = uc.lengthof();
	  for (int i=0; i<uc_len; i++)
	  {
		  str = mputCescaped(str, (char)unichar2int(uc[i].get_uchar()));
	  }
	  return str;
  }

  expstring_t mputOS(expstring_t str, const OCTETSTRING& os)
  {
	  int os_len = os.lengthof();
	  const unsigned char* os_str = (const unsigned char*)os;
	  for (int i=0; i<os_len; i++)
	  {
		  str = mputCescaped(str, os_str[i]);
	  }
	  return str;
  }

  expstring_t mputCS(expstring_t str, const CHARSTRING& cs)
  {
	  int cs_len = cs.lengthof();
	  const char* cs_str = (const char*)cs;
	  for (int i=0; i<cs_len; i++)
	  {
		  str = mputCescaped(str, cs_str[i]);
	  }
	  return str;
  }

  expstring_t mputXTDP_Requests(expstring_t xtdp_str, const XTDP__PDU__Defs::XTDP__Requests& pdu)
  {
	  xtdp_str = mputstr(xtdp_str, "<XTDP-Requests>\n");
	  int size = pdu.size_of();
	  for (int i=0; i<size; i++)
	  {
		  xtdp_str = mputstr(xtdp_str, "<XTDP-Request>\n");
		  const XTDP__PDU__Defs::XTDP__Request& request = pdu[i];
		  xtdp_str = mputprintf(xtdp_str, "<requestId>%d</requestId>\n"
		  "<widget>\n<widgetId>", (int)(request.requestId()));

		  xtdp_str = mputUC(xtdp_str, request.widget().widgetId());

		  xtdp_str = mputprintf(xtdp_str, "</widgetId>\n<widgetType>%s</widgetType>\n</widget>\n"
		  "<action>%s</action>\n"
		  "<argument>", request.widget().widgetType().enum_to_str(
			  (XTDP__PDU__Defs::XTDL__WidgetType::enum_type)(request.widget().widgetType()))+5 /* +5 to get rid of the "xdtp_" prefix */,
		  request.action__Field().enum_to_str((XTDP__PDU__Defs::XTDL__Actions::enum_type)(request.action__Field()))+5);
		  xtdp_str = mputCS(xtdp_str, request.argument());
		  xtdp_str = mputstr(xtdp_str, "</argument>\n");

		  if(request.argumentList().ispresent()) {
			  xtdp_str = mputstr(xtdp_str, "<argumentList>");

			  for(int i = 0; i < request.argumentList()().size_of(); i++) {
				  xtdp_str = mputstr(xtdp_str, "\n<argument>");
				  xtdp_str = mputstr(xtdp_str, request.argumentList()()[i]);
				  xtdp_str = mputstr(xtdp_str, "</argument>");
			  }

			  xtdp_str = mputstr(xtdp_str, "\n</argumentList>\n");
		  }

		  if (request.timeout__Field().ispresent())
		  {
			  xtdp_str = mputprintf(xtdp_str, "<timeout>%f</timeout>\n",
				  (double)((FLOAT)request.timeout__Field()));
		  }
		  xtdp_str = mputstr(xtdp_str, "</XTDP-Request>\n");
	  }
	  xtdp_str = mputstr(xtdp_str, "</XTDP-Requests>\n");
	  return xtdp_str;
  }

  expstring_t mputXTDP_Responses(expstring_t xtdp_str, const XTDP__PDU__Defs::XTDP__Responses& pdu)
  {
	  xtdp_str = mputstr(xtdp_str, "<XTDP-Responses>\n");
	  int size = pdu.size_of();
	  for (int i=0; i<size; i++)
	  {
		  xtdp_str = mputstr(xtdp_str, "<XTDP-Response>\n");
		  const XTDP__PDU__Defs::XTDP__Response& response = pdu[i];
		  const char* responseCodeEXERstrings[] = { "success", "pending", "error" };
		  xtdp_str = mputprintf(xtdp_str, "<requestId>%d</requestId>\n"
		  "<responseCode>%s</responseCode>\n",
		  (int)(response.requestId()),
		  responseCodeEXERstrings[(XTDP__PDU__Defs::XTDP__Result::enum_type)(response.responseCode())]);
		  if (response.argument().ispresent())
		  {
			  xtdp_str = mputstr(xtdp_str, "<argument>");
			  xtdp_str = mputUC(xtdp_str, (UNIVERSAL_CHARSTRING)(response.argument()));
			  xtdp_str = mputstr(xtdp_str, "</argument>\n");
		  }
		  if (response.errorSource().ispresent())
		  {
			  const XTDP__PDU__Defs::XTDP__ErrorSource& errorSource = (XTDP__PDU__Defs::XTDP__ErrorSource)(response.errorSource());
			  xtdp_str = mputprintf(xtdp_str, "<errorSource>%s</errorSource>\n",
				  errorSource.enum_to_str((XTDP__PDU__Defs::XTDP__ErrorSource::enum_type)errorSource)+5);
		  }
		  if (response.errorCode().ispresent())
		  {
			  const XTDP__PDU__Defs::XTDP__ErrorCode& errorCode = (XTDP__PDU__Defs::XTDP__ErrorCode)(response.errorCode());
			  xtdp_str = mputprintf(xtdp_str, "<errorCode>%s</errorCode>\n",
				  errorCode.enum_to_str(errorCode)+5);
		  }
		  if (response.errorMessage().ispresent())
		  {
			  xtdp_str = mputstr(xtdp_str, "<errorMessage>");
			  xtdp_str = mputCS(xtdp_str, (CHARSTRING)(response.errorMessage()));
			  xtdp_str = mputstr(xtdp_str, "</errorMessage>\n");
		  }
		  xtdp_str = mputstr(xtdp_str, "</XTDP-Response>\n");
	  }
	  xtdp_str = mputstr(xtdp_str, "</XTDP-Responses>\n");
          return xtdp_str;
  }

/*  expstring_t mputXTDP_Config(expstring_t xtdp_str, const XTDP__PDU__Defs::XTDL__Config& pdu)
  {
          // XTDL__Config == OCTETSTRING
          xtdp_str = mputstr(xtdp_str, (const char*)oct2str(pdu));
          return xtdp_str;
  }*/

  expstring_t mputXMLAddAttribute(expstring_t xtdp_str, const UNIVERSAL_CHARSTRING& attribute_name, const UNIVERSAL_CHARSTRING& attribute_value)
  {
        xtdp_str = mputUC(xtdp_str, attribute_name);
        xtdp_str = mputstr(xtdp_str, "=\"");
        xtdp_str = mputUC(xtdp_str, attribute_value);
        xtdp_str = mputstr(xtdp_str, "\"");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_Widgets(expstring_t xtdp_str, const XUL__XTDL::WidgetList& widgets)
  {
    for(int i = 0; i < widgets.size_of(); i++) {
      switch (widgets[i].get_selection()){
                case XUL__XTDL::AnyWidget::ALT_tabbox:{
                        xtdp_str = enc_XMLTag_Tabbox(xtdp_str, widgets[i].tabbox());
                        break;
                }
                case XUL__XTDL::AnyWidget::ALT_tree:{
                        xtdp_str = enc_XMLTag_Tree(xtdp_str, widgets[i].tree());
                        break;
                }
                case XUL__XTDL::AnyWidget::ALT_textlabel:{
                        xtdp_str = enc_XMLTag_Label(xtdp_str, widgets[i].textlabel());
                        break;
                }
                case XUL__XTDL::AnyWidget::ALT_spacer:{
                        xtdp_str = enc_XMLTag_Spacer(xtdp_str, widgets[i].spacer());
                        break;
                }
                case XUL__XTDL::AnyWidget::ALT_button:{
                        xtdp_str = enc_XMLTag_Button(xtdp_str, widgets[i].button());
                        break;
                }
                case XUL__XTDL::AnyWidget::ALT_textbox:{
                        xtdp_str = enc_XMLTag_Textbox(xtdp_str, widgets[i].textbox());
                        break;
                }
                case XUL__XTDL::AnyWidget::ALT_hbox:{
                        xtdp_str = enc_XMLTag_HBox(xtdp_str, widgets[i].hbox());
                        break;
                }
                case XUL__XTDL::AnyWidget::ALT_chart:{
                        xtdp_str = enc_XMLTag_Chart(xtdp_str, widgets[i].chart());
                        break;
                }
                case XUL__XTDL::AnyWidget::ALT_distributionchart:{
                        xtdp_str = enc_XMLTag_Distributionchart(xtdp_str, widgets[i].distributionchart());
                        break;
                }
                case XUL__XTDL::AnyWidget::ALT_toolbar:{
                        xtdp_str = enc_XMLTag_Toolbar(xtdp_str, widgets[i].toolbar());
                        break;
                }
                case XUL__XTDL::AnyWidget::ALT_toolbarbutton:{
                        xtdp_str = enc_XMLTag_Toolbarbutton(xtdp_str, widgets[i].toolbarbutton());
                        break;
                }
                case XUL__XTDL::AnyWidget::ALT_listbox:{
                        xtdp_str = enc_XMLTag_Listbox(xtdp_str, widgets[i].listbox());
                        break;
                }
                case XUL__XTDL::AnyWidget::ALT_numericalwidget:{
                        xtdp_str = enc_XMLTag_Numericalwidget(xtdp_str, widgets[i].numericalwidget());
                        break;
                }
                case XUL__XTDL::AnyWidget::ALT_menulist:{
                        xtdp_str = enc_XMLTag_Menulist(xtdp_str, widgets[i].menulist());
                        break;
                }
                default:
                        TTCN_error("Sending unknown <Widget>.");
        }
    }

    return xtdp_str;
  }

  expstring_t enc_XMLTag_Toolbarbutton(expstring_t xtdp_str, const XUL__XTDL::Toolbarbutton& toolbarbutton)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_TOOLBARBUTTON);

        //add attributes
        if (!isCorrectId(toolbarbutton.id()))
        {
            TTCN_error("Widget identifier must not contain space!");
        }
        xtdp_str = mputXMLAddAttribute(xtdp_str, " id", toolbarbutton.id());

        if (toolbarbutton.tooltiptext().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " tooltiptext", toolbarbutton.tooltiptext());

        if (toolbarbutton.disabled().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " disabled", toolbarbutton.disabled()() ? "true" : "false");

        if (toolbarbutton.imageId().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " imageid", toolbarbutton.imageId());

        xtdp_str = mputstr(xtdp_str, ">\n");

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_TOOLBARBUTTON">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_Separator(expstring_t xtdp_str, const XUL__XTDL::Separator& separator)
  {
      mputstr(xtdp_str, "<"XML_TAG_SEPARATOR);

      if (separator.id().ispresent())
      {
          if (!isCorrectId(separator.id()))
          {
              TTCN_error("Widget identifier must not contain space!");
          }
	      xtdp_str = mputXMLAddAttribute(xtdp_str, " id", separator.id());
      }

      mputstr(xtdp_str, "/>\n");

      return xtdp_str;
  }

  expstring_t enc_XMLTag_Window(expstring_t xtdp_str, const XUL__XTDL::Window& window)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_WINDOW);
        //add attributes
        if (window.height().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " height", float2str(window.height()));

        if (!isCorrectId(window.id()))
        {
            TTCN_error("Widget identifier must not contain space!");
        }
        xtdp_str = mputXMLAddAttribute(xtdp_str, " id", window.id());

        xtdp_str = mputXMLAddAttribute(xtdp_str, " orient", window.orient());

        if (window.title().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " title", window.title());

        if (window.width().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " width", float2str(window.width()));

        xtdp_str = mputstr(xtdp_str, ">\n");

        // encode images first, else they won't be defined if a button references them
        if(window.images().ispresent())
          for (int i = 0; i < window.images()().size_of(); i++)
            xtdp_str = enc_XMLTag_Image(xtdp_str, window.images()()[i]);

        xtdp_str = enc_XMLTag_Widgets(xtdp_str, window.widgets());

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_WINDOW">\n");
        return xtdp_str;
  }


  expstring_t enc_XMLTag_Tabbox(expstring_t xtdp_str, const XUL__XTDL::Tabbox& tabbox)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_TABBOX);


        if (tabbox.id().ispresent())
        {
            if (!isCorrectId(tabbox.id()))
            {
                TTCN_error("Widget identifier must not contain space!");
            }
            xtdp_str = mputXMLAddAttribute(xtdp_str, " id", tabbox.id());
        }
        if (tabbox.flex().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " flex", int2str(float2int(tabbox.flex())));
        if (tabbox.disabled().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " disabled", tabbox.disabled()() ? "true" : "false");
        if (tabbox.layout().ispresent()) {
    	    if(tabbox.layout() == XUL__XTDL::TabLayout::wrap)
    	    	xtdp_str = mputXMLAddAttribute(xtdp_str, " layout", "WRAP");
    	    else if(tabbox.layout() == XUL__XTDL::TabLayout::scroll)
    	    	xtdp_str = mputXMLAddAttribute(xtdp_str, " layout", "SCROLL");
    	    else if(tabbox.layout() == XUL__XTDL::TabLayout::card)
    	    	xtdp_str = mputXMLAddAttribute(xtdp_str, " layout", "CARD");
    	    else if(tabbox.layout() == XUL__XTDL::TabLayout::listcard)
    	    	xtdp_str = mputXMLAddAttribute(xtdp_str, " layout", "LISTCARD");
    	    else
    	    	TTCN_error("Encoding unknown <TabLayout>.");
        }

        xtdp_str = mputstr(xtdp_str, ">\n");
        //add content
        xtdp_str = enc_XMLTag_Tabs(xtdp_str, tabbox.tabs());

        xtdp_str = enc_XMLTag_TabPanels(xtdp_str, tabbox.tabpanels());

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_TABBOX">\n");
        return xtdp_str;
  }

  expstring_t enc_XMLTag_Tabs(expstring_t xtdp_str, const XUL__XTDL::Tabs& tabs)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_TABS);
        //add attributes

        xtdp_str = mputstr(xtdp_str, ">\n");
        //add content
        for (int i = 0; i < tabs.size_of(); i++)
                xtdp_str = enc_XMLTag_Tab(xtdp_str, tabs[i]);

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_TABS">\n");
        return xtdp_str;
  }

  expstring_t enc_XMLTag_Tab(expstring_t xtdp_str, const XUL__XTDL::Tab& tab)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_TAB);

        //add attributes
        if (tab.id().ispresent())
        {
            if (!isCorrectId(tab.id()))
            {
                TTCN_error("Widget identifier must not contain space!");
            }
            xtdp_str = mputXMLAddAttribute(xtdp_str, " id", tab.id());
        }

        xtdp_str = mputXMLAddAttribute(xtdp_str, " label", tab.tablabel());

        xtdp_str = mputstr(xtdp_str, ">\n");

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_TAB">\n");
        return xtdp_str;
  }


  expstring_t enc_XMLTag_TabPanels(expstring_t xtdp_str, const XUL__XTDL::Tabpanels& tabpanels)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_TABPANELS);


        xtdp_str = mputstr(xtdp_str, ">\n");
        //add content
        for (int i = 0; i < tabpanels.size_of(); i++)
                xtdp_str = enc_XMLTag_Tabpanel(xtdp_str, tabpanels[i]);

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_TABPANELS">\n");
        return xtdp_str;
  }

  expstring_t enc_XMLTag_Tabpanel(expstring_t xtdp_str, const XUL__XTDL::Tabpanel& tabpanel)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_TABPANEL);

        //add attributes
        if (tabpanel.id().ispresent())
        {
            if (!isCorrectId(tabpanel.id()))
            {
                TTCN_error("Widget identifier must not contain space!");
            }
            xtdp_str = mputXMLAddAttribute(xtdp_str, " id", tabpanel.id());
        }

        if (tabpanel.maxheight().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " maxheight", float2str(tabpanel.maxheight()));

        xtdp_str = mputXMLAddAttribute(xtdp_str, " orient", tabpanel.orient());

        xtdp_str = mputstr(xtdp_str, ">\n");

        //add widgets
        xtdp_str = enc_XMLTag_Widgets(xtdp_str, tabpanel.widgets());

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_TABPANEL">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_Tree(expstring_t xtdp_str, const XUL__XTDL::Tree& tree)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_TREE);

        //add attributes
        if (tree.hidecolumnpicker().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " hidecolumnpicker", tree.hidecolumnpicker()() ? "true" : "false");

        if (!isCorrectId(tree.id()))
        {
            TTCN_error("Widget identifier must not contain space!");
        }
        xtdp_str = mputXMLAddAttribute(xtdp_str, " id", tree.id());

        if (tree.flex().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " flex", int2str(float2int(tree.flex())));
        if (tree.disabled().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " disabled", tree.disabled()() ? "true" : "false");

        xtdp_str = mputXMLAddAttribute(xtdp_str, " rows", float2str(tree.rows()));

        xtdp_str = mputstr(xtdp_str, ">\n");

        xtdp_str = enc_XMLTag_TreeCols(xtdp_str, tree.treecols());

        xtdp_str = enc_XMLTag_TreeChildren(xtdp_str, tree.treechildren());

        //add content

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_TREE">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_TreeCols(expstring_t xtdp_str, const XUL__XTDL::Treecols& treecols)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_TREECOLS">\n");

        //add content
        for (int i = 0; i < treecols.size_of(); i++)
                xtdp_str = enc_XMLTag_TreeCol(xtdp_str, treecols[i]);

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_TREECOLS">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_TreeCol(expstring_t xtdp_str, const XUL__XTDL::Treecol& treecol)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_TREECOL);

        //add attributes
        if (treecol.editable().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " editable", treecol.editable()() ? "true" : "false");

        if (treecol.flex().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " flex", int2str(float2int(treecol.flex())));

        if (treecol.id().ispresent())
        {
            if (!isCorrectId(treecol.id()))
            {
                TTCN_error("Widget identifier must not contain space!");
            }
            xtdp_str = mputXMLAddAttribute(xtdp_str, " id", treecol.id());
        }

        xtdp_str = mputXMLAddAttribute(xtdp_str, " label", treecol.columnlabel());

        xtdp_str = mputXMLAddAttribute(xtdp_str, " widgetType", treecol.widgetType().enum_to_str((XUL__XTDL::WidgetType::enum_type)treecol.widgetType()));

        xtdp_str = mputstr(xtdp_str, ">\n");

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_TREECOL">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_TreeChildren(expstring_t xtdp_str, const XUL__XTDL::Treechildren& treechildren)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_TREECHILDREN">\n");

        //add content
        for (int i = 0; i < treechildren.size_of(); i++)
               xtdp_str = enc_XMLTag_TreeItem(xtdp_str, treechildren[i]);

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_TREECHILDREN">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_TreeItem(expstring_t xtdp_str, const XUL__XTDL::Treeitem& treeitem)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_TREEITEM">\n");

        //add content
        for (int i = 0; i < treeitem.size_of(); i++)
                xtdp_str = enc_XMLTag_TreeRow(xtdp_str, treeitem[i]);

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_TREEITEM">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_TreeRow(expstring_t xtdp_str, const XUL__XTDL::Treerow& treerow)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_TREEROW">\n");

        //add content
        for (int i = 0; i < treerow.size_of(); i++)
                xtdp_str = enc_XMLTag_TreeCell(xtdp_str, treerow[i]);

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_TREEROW">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_TreeCell(expstring_t xtdp_str, const XUL__XTDL::Treecell& treecell)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_TREECELL);

        //add attributes
        if (treecell.id().ispresent())
        {
            if (!isCorrectId(treecell.id()))
            {
                TTCN_error("Widget identifier must not contain space!");
            }
            xtdp_str = mputXMLAddAttribute(xtdp_str, " id", treecell.id());
        }

        xtdp_str = mputXMLAddAttribute(xtdp_str, " label", treecell.textlabel());

        if (treecell.tooltiptext().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " tooltiptext", treecell.tooltiptext());

        xtdp_str = mputstr(xtdp_str, ">\n");

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_TREECELL">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_HBox(expstring_t xtdp_str, const XUL__XTDL::Hbox& hbox)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_HBOX);

        //add attributes
        if (hbox.id().ispresent())
        {
            if (!isCorrectId(hbox.id()))
            {
                TTCN_error("Widget identifier must not contain space!");
            }
            xtdp_str = mputXMLAddAttribute(xtdp_str, " id", hbox.id());
        }

        if (hbox.flex().ispresent())
        {
            xtdp_str = mputXMLAddAttribute(xtdp_str, " flex", int2str(float2int(hbox.flex())));
        }

        if (hbox.disabled().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " disabled", hbox.disabled()() ? "true" : "false");

        if (hbox.orient().ispresent())
                 xtdp_str = mputXMLAddAttribute(xtdp_str, " orient", hbox.orient());

        xtdp_str = mputstr(xtdp_str, ">\n");

        //add widgets
        xtdp_str = enc_XMLTag_Widgets(xtdp_str, hbox.widgets());

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_HBOX">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_Label(expstring_t xtdp_str, const XUL__XTDL::Label& label)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_LABEL);

        //add attributes
        if (label.id().ispresent())
        {
            if (!isCorrectId(label.id()))
            {
                TTCN_error("Widget identifier must not contain space!");
            }
            xtdp_str = mputXMLAddAttribute(xtdp_str, " id", label.id());
        }

        if (label.flex().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " flex", int2str(float2int(label.flex())));

        if (label.disabled().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " disabled", label.disabled()() ? "true" : "false");

        if (label.style().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " style", label.style());

        xtdp_str = mputXMLAddAttribute(xtdp_str, " value", label.textvalue());

        xtdp_str = mputstr(xtdp_str, ">\n");

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_LABEL">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_Spacer(expstring_t xtdp_str, const XUL__XTDL::Spacer& spacer)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_SPACER);

        //add attributes
        xtdp_str = mputXMLAddAttribute(xtdp_str, " flex", int2str(float2int(spacer.flex())));

        if (spacer.id().ispresent())
        {
            if (!isCorrectId(spacer.id()))
            {
                TTCN_error("Widget identifier must not contain space!");
            }
            xtdp_str = mputXMLAddAttribute(xtdp_str, " id", spacer.id());
        }

        xtdp_str = mputstr(xtdp_str, ">\n");

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_SPACER">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_Button(expstring_t xtdp_str, const XUL__XTDL::Button& button)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_BUTTON);

        //add attributes
        if (button.checked().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " checked", button.checked()() ? "true" : "false");

        if (button.disabled().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " disabled", button.disabled()() ? "true" : "false");

        if (!isCorrectId(button.id()))
        {
            TTCN_error("Widget identifier must not contain space!");
        }
        xtdp_str = mputXMLAddAttribute(xtdp_str, " id", button.id());

        if (button.flex().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " flex", int2str(float2int(button.flex())));

        xtdp_str = mputXMLAddAttribute(xtdp_str, " label", button.buttonlabel());

        if (button.buttontype().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " type", button.buttontype());

        if (button.imageId().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " imageid", button.imageId());

        xtdp_str = mputstr(xtdp_str, ">\n");

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_BUTTON">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_Textbox(expstring_t xtdp_str, const XUL__XTDL::Textbox& textbox)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_TEXTBOX);

        //add attributes
        if (textbox.disabled().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " disabled", textbox.disabled()() ? "true" : "false");

        if (textbox.flex().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " flex", int2str(float2int(textbox.flex())));

        if (!isCorrectId(textbox.id()))
        {
            TTCN_error("Widget identifier must not contain space!");
        }
        xtdp_str = mputXMLAddAttribute(xtdp_str, " id", textbox.id());

        if (textbox.multiline().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " multiline", textbox.multiline()() ? "true" : "false");

        if (textbox.readonly().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " readonly", textbox.readonly()() ? "true" : "false");

        if (textbox.rows().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " rows", float2str(textbox.rows()));

        if (textbox.textvalue().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " value", textbox.textvalue());

        if (textbox.widgetType().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " widgetType", textbox.widgetType());


        if (textbox.wrap().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " wrap", textbox.wrap()() ? "true" : "false");

        xtdp_str = mputstr(xtdp_str, ">\n");

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_TEXTBOX">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_Chart(expstring_t xtdp_str, const XUL__XTDL::Chart& chart)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_CHART);

        if (!isCorrectId(chart.id()))
        {
            TTCN_error("Widget identifier must not contain space!");
        }
        xtdp_str = mputXMLAddAttribute(xtdp_str, " id", chart.id());

        if (chart.flex().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " flex", int2str(float2int(chart.flex())));

        xtdp_str = mputXMLAddAttribute(xtdp_str, " title", chart.title());

        if(chart.disabled().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " disabled", chart.disabled()() ? "true" : "false");

        if(chart.zoomable().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " zoomable", chart.zoomable()() ? "true" : "false");

        if(chart.axisXType().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " axisXType", chart.axisXType()());

        if(chart.axisYType().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " axisYType", chart.axisYType()());

        if(chart.gridX().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " gridX", chart.gridX()() ? "true" : "false");

        if(chart.gridY().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " gridY", chart.gridY()() ? "true" : "false");

        if(chart.foregroundColor().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " foregroundColor", chart.foregroundColor()());

        if(chart.backgroundColor().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " backgroundColor", chart.backgroundColor()());

        if(chart.gridColor().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " gridColor", chart.gridColor()());

        xtdp_str = mputstr(xtdp_str, ">\n");
        //add content
        for (int i = 0; i < chart.traceList().size_of(); i++)
                xtdp_str = enc_XMLTag_Trace(xtdp_str, chart.traceList()[i]);

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_CHART">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_Distributionchart(expstring_t xtdp_str, const XUL__XTDL::Distributionchart& chart)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_DISTRIBUTIONCHART);

        if (!isCorrectId(chart.id()))
        {
            TTCN_error("Widget identifier must not contain space!");
        }
        xtdp_str = mputXMLAddAttribute(xtdp_str, " id", chart.id());

        if (chart.flex().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " flex", int2str(float2int(chart.flex())));

        xtdp_str = mputXMLAddAttribute(xtdp_str, " title", chart.title());

        if(chart.disabled().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " disabled", chart.disabled()() ? "true" : "false");

        if(chart.axisXLabel().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " axisXLabel", chart.axisXLabel()());

        if(chart.axisYLabel().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " axisYLabel", chart.axisYLabel()());

        if(chart.showlegend().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " showlegend", chart.showlegend()() ? "true" : "false");

        if(chart.orientation().ispresent()) {
    	    if(chart.orientation() == XUL__XTDL::XULOrientation::vertical)
    	    	xtdp_str = mputXMLAddAttribute(xtdp_str, " orientation", "VERTICAL");
    	    else if(chart.orientation() == XUL__XTDL::XULOrientation::horizontal)
    	    	xtdp_str = mputXMLAddAttribute(xtdp_str, " orientation", "HORIZONTAL");
    	    else
    	    	TTCN_error("Encoding unknown <orientation>.");
        }

        if(chart.backgroundcolor().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " backgroundcolor", chart.backgroundcolor()());

        if(chart.gridcolor().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " gridcolor", chart.gridcolor()());

        if(chart.labeltype().ispresent()) {
    	    if(chart.labeltype() == XUL__XTDL::DistributionchartLabeltype::absolute)
    	    	xtdp_str = mputXMLAddAttribute(xtdp_str, " labeltype", "ABSOLUTE");
    	    else if(chart.labeltype() == XUL__XTDL::DistributionchartLabeltype::mixedType)
    	    	xtdp_str = mputXMLAddAttribute(xtdp_str, " labeltype", "MIXED");
    	    else if(chart.labeltype() == XUL__XTDL::DistributionchartLabeltype::noneType)
    	    	xtdp_str = mputXMLAddAttribute(xtdp_str, " labeltype", "NONE");
    	    else if(chart.labeltype() == XUL__XTDL::DistributionchartLabeltype::percentage)
    	    	xtdp_str = mputXMLAddAttribute(xtdp_str, " labeltype", "PERCENTAGE");
    	    else
    	    	TTCN_error("Encoding unknown <labeltype>.");
        }

        xtdp_str = mputstr(xtdp_str, ">\n<"XML_TAG_INTERVALLIMITS);
        xtdp_str = mputXMLAddAttribute(xtdp_str, " id", chart.intervallimits().id());
        xtdp_str = mputstr(xtdp_str, ">");
        //add intervallimits
        for (int i = 0; i < chart.intervallimits().values().size_of(); i++) {
        	xtdp_str = mputstr(xtdp_str, "\n<value>");
        	xtdp_str = mputstr(xtdp_str, float2str(chart.intervallimits().values()[i]));
        	xtdp_str = mputstr(xtdp_str, "</value>");
        }
        xtdp_str = mputstr(xtdp_str, "\n</"XML_TAG_INTERVALLIMITS">\n");

        //add valuelists
        for (int i = 0; i < chart.valuelist().size_of(); i++) {
          xtdp_str = enc_XMLTag_Valuelist(xtdp_str, chart.valuelist()[i]);
        }

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_DISTRIBUTIONCHART">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_Valuelist(expstring_t xtdp_str, const XUL__XTDL::DistributionchartValuelist& valuelist)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_VALUELIST);
        if (!isCorrectId(valuelist.id()))
        {
            TTCN_error("Widget identifier must not contain space!");
        }
    	xtdp_str = mputXMLAddAttribute(xtdp_str, " id", valuelist.id());

    	if(valuelist.color().ispresent())
    		xtdp_str = mputXMLAddAttribute(xtdp_str, " color", valuelist.color());
    	if(valuelist.legend().ispresent())
    		xtdp_str = mputXMLAddAttribute(xtdp_str, " legend", valuelist.legend());

        if(valuelist.labeltype().ispresent()) {
    	    if(valuelist.labeltype() == XUL__XTDL::DistributionchartLabeltype::absolute)
    	    	xtdp_str = mputXMLAddAttribute(xtdp_str, " labeltype", "ABSOLUTE");
    	    else if(valuelist.labeltype() == XUL__XTDL::DistributionchartLabeltype::mixedType)
    	    	xtdp_str = mputXMLAddAttribute(xtdp_str, " labeltype", "MIXED");
    	    else if(valuelist.labeltype() == XUL__XTDL::DistributionchartLabeltype::noneType)
    	    	xtdp_str = mputXMLAddAttribute(xtdp_str, " labeltype", "NONE");
    	    else if(valuelist.labeltype() == XUL__XTDL::DistributionchartLabeltype::percentage)
    	    	xtdp_str = mputXMLAddAttribute(xtdp_str, " labeltype", "PERCENTAGE");
    	    else
    	    	TTCN_error("Encoding unknown <labeltype>.");
        }

        xtdp_str = mputstr(xtdp_str, ">");

    	for (int j = 0; j < valuelist.values().size_of(); j++) {
        	xtdp_str = mputstr(xtdp_str, "\n<value>");
        	xtdp_str = mputstr(xtdp_str, int2str(valuelist.values()[j]));
        	xtdp_str = mputstr(xtdp_str, "</value>");
    	}

    	xtdp_str = mputstr(xtdp_str, "\n</"XML_TAG_VALUELIST">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_Trace(expstring_t xtdp_str, const XUL__XTDL::Trace& trace)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_TRACE);

        if (!isCorrectId(trace.id()))
        {
            TTCN_error("Widget identifier must not contain space!");
        }
        xtdp_str = mputXMLAddAttribute(xtdp_str, " id", trace.id());

        xtdp_str = mputXMLAddAttribute(xtdp_str, " name", trace.name());

        if(trace.maxPoints().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " maxPoints", int2str(trace.maxPoints()()));

        if(trace.color().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " color", trace.color()());

        if(trace.fill().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " fill", trace.fill()() ? "true" : "false");

        if(trace.physicalUnitX().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " physicalUnitX", trace.physicalUnitX()());

        if(trace.physicalUnitY().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " physicalUnitY", trace.physicalUnitY()());

        xtdp_str = mputstr(xtdp_str, "/>\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_Toolbar(expstring_t xtdp_str, const XUL__XTDL::Toolbar& toolbar)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_TOOLBAR);

        //add attributes
        if (toolbar.disabled().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " disabled", toolbar.disabled()() ? "true" : "false");

        if (!isCorrectId(toolbar.id()))
        {
            TTCN_error("Widget identifier must not contain space!");
        }
        xtdp_str = mputXMLAddAttribute(xtdp_str, " id", toolbar.id());

        if(toolbar.placement().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " placement", toolbar.placement());

        xtdp_str = mputstr(xtdp_str, ">\n");

        //add toolbarbuttons
        for (int i = 0; i < toolbar.toolbarelements().size_of(); i++) {
        	if(toolbar.toolbarelements()[i].get_selection() == XUL__XTDL::Toolbarelement::ALT_toolbarbutton)
            xtdp_str = enc_XMLTag_Toolbarbutton(xtdp_str, toolbar.toolbarelements()[i].toolbarbutton());
          else if(toolbar.toolbarelements()[i].get_selection() == XUL__XTDL::Toolbarelement::ALT_separator)
            xtdp_str = enc_XMLTag_Separator(xtdp_str, toolbar.toolbarelements()[i].separator());
        }

        xtdp_str = mputstr(xtdp_str, ">\n");

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_TOOLBAR">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_Image(expstring_t xtdp_str, const XUL__XTDL::Image& image)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_IMAGE);

        //add attributes
        if (!isCorrectId(image.id()))
        {
            TTCN_error("Widget identifier must not contain space!");
        }
        xtdp_str = mputXMLAddAttribute(xtdp_str, " id", image.id());

        if (image.imageSource().get_selection() == XUL__XTDL::ImageSource::ALT_imageurl)
                xtdp_str = mputXMLAddAttribute(xtdp_str, " imageurl", image.imageSource().imageurl());
        else if (image.imageSource().get_selection() == XUL__XTDL::ImageSource::ALT_imagedata)
                xtdp_str = mputXMLAddAttribute(xtdp_str, " imagedata", image.imageSource().imagedata());
        else
                TTCN_error("Encoding unknown <ImageSource>.");

        xtdp_str = mputstr(xtdp_str, ">\n");

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_IMAGE">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_Listbox(expstring_t xtdp_str, const XUL__XTDL::Listbox& listbox)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_LISTBOX);

        //add attributes
        if (!isCorrectId(listbox.id()))
        {
            TTCN_error("Widget identifier must not contain space!");
        }
        xtdp_str = mputXMLAddAttribute(xtdp_str, " id", listbox.id());

        if (listbox.flex().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " flex", int2str(float2int(listbox.flex())));

        if (listbox.disabled().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " disabled", listbox.disabled()() ? "true" : "false");

        xtdp_str = mputXMLAddAttribute(xtdp_str, " rows", int2str(listbox.rows()));

        if(listbox.seltype().ispresent()) {
          if (listbox.seltype() == XUL__XTDL::SelType::single)
            xtdp_str = mputXMLAddAttribute(xtdp_str, " seltype", "single");
          else if (listbox.seltype() == XUL__XTDL::SelType::multiple)
            xtdp_str = mputXMLAddAttribute(xtdp_str, " seltype", "multiple");
          else
            TTCN_error("Encoding unknown seltype.");
        }

        xtdp_str = mputstr(xtdp_str, ">\n");

        for (int i = 0; i < listbox.listitems().size_of(); i++) {
          xtdp_str = enc_XMLTag_Listitem(xtdp_str, listbox.listitems()[i]);
        }

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_LISTBOX">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_Listitem(expstring_t xtdp_str, const XUL__XTDL::Listitem& listitem)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_LISTITEM);

        //add attributes
        if (!isCorrectId(listitem.id()))
        {
            TTCN_error("Widget identifier must not contain space!");
        }
        xtdp_str = mputXMLAddAttribute(xtdp_str, " id", listitem.id());

        xtdp_str = mputXMLAddAttribute(xtdp_str, " label", listitem.textlabel());

        if (listitem.selected().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " selected", listitem.selected()() ? "true" : "false");

        xtdp_str = mputstr(xtdp_str, ">\n");

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_LISTITEM">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_Numericalwidget(expstring_t xtdp_str, const XUL__XTDL::Numericalwidget& numericalwidget)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_NUMERICALWIDGET);

        //add attributes
        if (!isCorrectId(numericalwidget.id()))
        {
            TTCN_error("Widget identifier must not contain space!");
        }
        xtdp_str = mputXMLAddAttribute(xtdp_str, " id", numericalwidget.id());

        if (numericalwidget.flex().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " flex", int2str(float2int(numericalwidget.flex())));

        if (numericalwidget.disabled().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " disabled", numericalwidget.disabled()() ? "true" : "false");

        xtdp_str = mputXMLAddAttribute(xtdp_str, " widgetType",
          numericalwidget.widgetType().enum_to_str((XUL__XTDL::NumericalWidgetType::enum_type)numericalwidget.widgetType()));

        if(numericalwidget.widgetType() == XUL__XTDL::NumericalWidgetType::floatField)
          xtdp_str = mputXMLAddAttribute(xtdp_str, " value", float2str(numericalwidget.numericvalue()));
        else if(numericalwidget.widgetType() == XUL__XTDL::NumericalWidgetType::integerField)
          xtdp_str = mputXMLAddAttribute(xtdp_str, " value", int2str(float2int(numericalwidget.numericvalue())));
        else
          TTCN_error("Encoding unknown numericalwidget type");

        if (numericalwidget.readonly().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " readonly", numericalwidget.readonly()() ? "true" : "false");

        if (numericalwidget.spinner().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " spinner", numericalwidget.spinner()() ? "true" : "false");

        if (numericalwidget.slider().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " slider", numericalwidget.slider()() ? "true" : "false");

        if(numericalwidget.widgetType() == XUL__XTDL::NumericalWidgetType::floatField) {
          if (numericalwidget.minvalue().ispresent())
                  xtdp_str = mputXMLAddAttribute(xtdp_str, " minvalue", float2str(numericalwidget.minvalue()));

          if (numericalwidget.maxvalue().ispresent())
                  xtdp_str = mputXMLAddAttribute(xtdp_str, " maxvalue", float2str(numericalwidget.maxvalue()));

          if (numericalwidget.stepsize().ispresent())
                  xtdp_str = mputXMLAddAttribute(xtdp_str, " stepsize", float2str(numericalwidget.stepsize()));
        }
        else if(numericalwidget.widgetType() == XUL__XTDL::NumericalWidgetType::integerField) {
          if (numericalwidget.minvalue().ispresent())
                  xtdp_str = mputXMLAddAttribute(xtdp_str, " minvalue", int2str(float2int(numericalwidget.minvalue())));

          if (numericalwidget.maxvalue().ispresent())
                  xtdp_str = mputXMLAddAttribute(xtdp_str, " maxvalue", int2str(float2int(numericalwidget.maxvalue())));

          if (numericalwidget.stepsize().ispresent())
                  xtdp_str = mputXMLAddAttribute(xtdp_str, " stepsize", int2str(float2int(numericalwidget.stepsize())));
        }
        else
          TTCN_error("Encoding unknown numericalwidget type");

        xtdp_str = mputstr(xtdp_str, ">\n");

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_NUMERICALWIDGET">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_Menulist(expstring_t xtdp_str, const XUL__XTDL::Menulist& menulist)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_MENULIST);

        //add attributes
        if (!isCorrectId(menulist.id()))
        {
            TTCN_error("Widget identifier must not contain space!");
        }
        xtdp_str = mputXMLAddAttribute(xtdp_str, " id", menulist.id());

        if (menulist.flex().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " flex", int2str(float2int(menulist.flex())));

        if (menulist.disabled().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " disabled", menulist.disabled()() ? "true" : "false");

        if (menulist.editable().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " editable", menulist.editable()() ? "true" : "false");

        if (menulist.menulabel().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " label", menulist.menulabel());

        xtdp_str = mputstr(xtdp_str, ">\n");

        if(menulist.menuitems().size_of() > 0) {

          xtdp_str = mputstr(xtdp_str, "<"XML_TAG_MENUPOPUP">\n");

          for (int i = 0; i < menulist.menuitems().size_of(); i++) {
            xtdp_str = enc_XMLTag_Menuitem(xtdp_str, menulist.menuitems()[i]);
          }

          xtdp_str = mputstr(xtdp_str, "</"XML_TAG_MENUPOPUP">\n");
        }

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_MENULIST">\n");

        return xtdp_str;
  }

  expstring_t enc_XMLTag_Menuitem(expstring_t xtdp_str, const XUL__XTDL::Menuitem& menuitem)
  {
        xtdp_str = mputstr(xtdp_str, "<"XML_TAG_MENUITEM);

        //add attributes
        if (!isCorrectId(menuitem.id()))
        {
            TTCN_error("Widget identifier must not contain space!");
        }
        xtdp_str = mputXMLAddAttribute(xtdp_str, " id", menuitem.id());

        xtdp_str = mputXMLAddAttribute(xtdp_str, " label", menuitem.textlabel());

        if (menuitem.selected().ispresent())
                xtdp_str = mputXMLAddAttribute(xtdp_str, " selected", menuitem.selected()() ? "true" : "false");

        xtdp_str = mputstr(xtdp_str, ">\n");

        xtdp_str = mputstr(xtdp_str, "</"XML_TAG_MENUITEM">\n");

        return xtdp_str;
  }

  CHARSTRING enc__XUL(const XTDP__PDU__Defs::XTDP__XML__Tag& tag)
  {
  	if (TTCN_Logger::log_this_event(TTCN_DEBUG)) {
  		TTCN_Logger::begin_event(TTCN_DEBUG);
  		TTCN_Logger::log_event("enc__XUL() called with argument: ");
  		tag.log();
  		TTCN_Logger::end_event();
  	}

        expstring_t xtdp_str = memptystr();

        switch (tag.get_selection()) {
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__window:{
                        xtdp_str = enc_XMLTag_Window(xtdp_str, tag.xtdp__window());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__tabbox:{
                        xtdp_str = enc_XMLTag_Tabbox(xtdp_str, tag.xtdp__tabbox());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__tabs:{
                        xtdp_str = enc_XMLTag_Tabs(xtdp_str, tag.xtdp__tabs());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__tab:{
                        xtdp_str = enc_XMLTag_Tab(xtdp_str, tag.xtdp__tab());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__tabpanels:{
                        xtdp_str = enc_XMLTag_TabPanels(xtdp_str, tag.xtdp__tabpanels());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__tabpanel:{
                        xtdp_str = enc_XMLTag_Tabpanel(xtdp_str, tag.xtdp__tabpanel());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__tree:{
                        xtdp_str = enc_XMLTag_Tree(xtdp_str, tag.xtdp__tree());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__treecols:{
                        xtdp_str = enc_XMLTag_TreeCols(xtdp_str, tag.xtdp__treecols());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__treecol:{
                        xtdp_str = enc_XMLTag_TreeCol(xtdp_str, tag.xtdp__treecol());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__treechildren:{
                	    xtdp_str = enc_XMLTag_TreeChildren(xtdp_str, tag.xtdp__treechildren());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__treeitem:{
                        xtdp_str = enc_XMLTag_TreeItem(xtdp_str, tag.xtdp__treeitem());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__treerow:{
                        xtdp_str = enc_XMLTag_TreeRow(xtdp_str, tag.xtdp__treerow());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__treecell:{
                        xtdp_str = enc_XMLTag_TreeCell(xtdp_str, tag.xtdp__treecell());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__hbox:{
                        xtdp_str = enc_XMLTag_HBox(xtdp_str, tag.xtdp__hbox());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__labeltype:{
                        xtdp_str = enc_XMLTag_Label(xtdp_str, tag.xtdp__labeltype());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__spacer:{
                        xtdp_str = enc_XMLTag_Spacer(xtdp_str, tag.xtdp__spacer());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__button:{
                        xtdp_str = enc_XMLTag_Button(xtdp_str, tag.xtdp__button());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__textbox:{
                        xtdp_str = enc_XMLTag_Textbox(xtdp_str, tag.xtdp__textbox());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__chart:{
                        xtdp_str = enc_XMLTag_Chart(xtdp_str, tag.xtdp__chart());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__distributionchart:{
                        xtdp_str = enc_XMLTag_Distributionchart(xtdp_str, tag.xtdp__distributionchart());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__valuelist:{
                        xtdp_str = enc_XMLTag_Valuelist(xtdp_str, tag.xtdp__valuelist());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__tracelist:{
                        for (int i = 0; i < tag.xtdp__tracelist().size_of(); i++)
                               xtdp_str = enc_XMLTag_Trace(xtdp_str, tag.xtdp__tracelist()[i]);
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__trace:{
                        xtdp_str = enc_XMLTag_Trace(xtdp_str, tag.xtdp__trace());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__toolbar:{
                        xtdp_str = enc_XMLTag_Toolbar(xtdp_str, tag.xtdp__toolbar());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__toolbarbutton:{
                        xtdp_str = enc_XMLTag_Toolbarbutton(xtdp_str, tag.xtdp__toolbarbutton());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__image:{
                        xtdp_str = enc_XMLTag_Image(xtdp_str, tag.xtdp__image());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__listbox:{
                        xtdp_str = enc_XMLTag_Listbox(xtdp_str, tag.xtdp__listbox());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__listitem:{
                        xtdp_str = enc_XMLTag_Listitem(xtdp_str, tag.xtdp__listitem());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__numericalwidget:{
                        xtdp_str = enc_XMLTag_Numericalwidget(xtdp_str, tag.xtdp__numericalwidget());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__menulist:{
                        xtdp_str = enc_XMLTag_Menulist(xtdp_str, tag.xtdp__menulist());
                        break;
                }
                case XTDP__PDU__Defs::XTDP__XML__Tag::ALT_xtdp__menuitem:{
                        xtdp_str = enc_XMLTag_Menuitem(xtdp_str, tag.xtdp__menuitem());
                        break;
                }
                default:
                        TTCN_error("Sending unknown <XTDP-XML-Tag>.");
        }

        CHARSTRING ret_val(mstrlen(xtdp_str),(const char*)xtdp_str);

        if (TTCN_Logger::log_this_event(TTCN_DEBUG)) {
        	TTCN_Logger::begin_event(TTCN_DEBUG);
        	TTCN_Logger::log_event("enc__XUL() return value: ");
        	ret_val.log();
        	TTCN_Logger::end_event();
        }

        Free(xtdp_str);

        return ret_val;
  }

  expstring_t mputXTDP_Add(expstring_t xtdp_str, const XTDP__PDU__Defs::XTDP__AddRequests& pdu)
  {
        xtdp_str = mputstr(xtdp_str, "<XTDP-AddRequests>\n");

        for (int i =0; i < pdu.size_of(); i++){
                  xtdp_str = mputstr(xtdp_str, "<XTDP-AddRequest>\n");

                  const XTDP__PDU__Defs::XTDP__AddRequest& request = pdu[i];

                  xtdp_str = mputprintf(xtdp_str, "<requestId>%d</requestId>\n",(int)(request.requestId()));

                  if (request.parent__widgetID().ispresent()) {
                      xtdp_str = mputprintf(xtdp_str, "<parentWidgetId>");
		  xtdp_str = mputUC(xtdp_str, request.parent__widgetID()());
                      xtdp_str = mputprintf(xtdp_str, "</parentWidgetId>\n");
                  }

                  xtdp_str = mputstr(xtdp_str, (const char*)enc__XUL(request.xul()));

                  xtdp_str = mputstr(xtdp_str, "</XTDP-AddRequest>\n");

        }

        xtdp_str = mputstr(xtdp_str, "</XTDP-AddRequests>\n");
        return xtdp_str;
  }

  expstring_t mputXTDP_Remove(expstring_t xtdp_str, const XTDP__PDU__Defs::XTDP__RemoveRequests& pdu)
  {
        xtdp_str = mputstr(xtdp_str, "<XTDP-RemoveRequests>\n");

        for (int i =0; i < pdu.size_of(); i++){
                xtdp_str = mputstr(xtdp_str, "<XTDP-RemoveRequest>\n");

                const XTDP__PDU__Defs::XTDP__RemoveRequest& request = pdu[i];
                xtdp_str = mputprintf(xtdp_str, "<requestId>%d</requestId>\n",
                                        (int)request.requestId());
                if(request.widgetID().ispresent()) {
                    xtdp_str = mputprintf(xtdp_str, "<widgetId>");
                    xtdp_str = mputUC(xtdp_str, request.widgetID()());
                    xtdp_str = mputprintf(xtdp_str, "</widgetId>\n");
                }

                xtdp_str = mputstr(xtdp_str, "</XTDP-RemoveRequest>\n");
        }

        xtdp_str = mputstr(xtdp_str, "</XTDP-RemoveRequests>\n");
        return xtdp_str;
  }

  expstring_t mputXTDP_LayoutRequest(expstring_t xtdp_str, const XTDP__PDU__Defs::XTDP__LayoutRequest& pdu)
  {
        xtdp_str = mputstr(xtdp_str, "<XTDP-LayoutRequest>\n");

        xtdp_str = mputprintf(xtdp_str, "<requestId>%d</requestId>\n", (int)pdu.requestId());

        if (pdu.widgetID().ispresent()) {
            xtdp_str = mputprintf(xtdp_str, "<widgetId>");
            xtdp_str = mputUC(xtdp_str, pdu.widgetID()());
            xtdp_str = mputprintf(xtdp_str, "</widgetId>\n");
        }

        xtdp_str = mputstr(xtdp_str, "</XTDP-LayoutRequest>\n");
        return xtdp_str;
  }

  expstring_t mputXTDP_AuthChallenge(expstring_t xtdp_str, const XTDP__PDU__Defs::XTDP__AuthChallenge& pdu)
  {
        xtdp_str = mputstr(xtdp_str, "<XTDP-AuthChallenge>\n");

        xtdp_str = mputUC(xtdp_str, pdu);

        xtdp_str = mputstr(xtdp_str, "</XTDP-AuthChallenge>\n");
        return xtdp_str;
  }

  CHARSTRING enc__XTDP__Message(const XTDP__PDU__Defs::XTDP__Message& pdu)
  {
          if (TTCN_Logger::log_this_event(TTCN_DEBUG)) {
          	TTCN_Logger::begin_event(TTCN_DEBUG);
                  	TTCN_Logger::log_event("enc__XTDP__Message() called with argument: ");
                  	pdu.log();
		TTCN_Logger::end_event();
	}
	  expstring_t xtdp_str = mcopystr("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
	  "<XTDP-Message");

	  if (pdu.noNamespaceSchemaLocation().ispresent())
	  {
		  xtdp_str = mputstr(xtdp_str,
		  " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
		  "xsi:noNamespaceSchemaLocation=\"");
		  xtdp_str = mputUC(xtdp_str,
			  (UNIVERSAL_CHARSTRING)(pdu.noNamespaceSchemaLocation()) );
		  xtdp_str = mputstr(xtdp_str, "\">\n");
	  }
	  else
		  xtdp_str = mputstr(xtdp_str, ">\n");
	  switch (pdu.xtdp__Message().get_selection())
	  {
	  case XTDP__PDU__Defs::XTDP__Message_xtdp__Message::UNBOUND_VALUE:
		  TTCN_error("Sending unbound XTDP-Message.xtdp-Message");
		  break;
	  case XTDP__PDU__Defs::XTDP__Message_xtdp__Message::ALT_xtdp__Requests:
		  xtdp_str = mputXTDP_Requests(xtdp_str, pdu.xtdp__Message().xtdp__Requests());
		  break;
          case XTDP__PDU__Defs::XTDP__Message_xtdp__Message::ALT_xtdp__Responses:
                  xtdp_str = mputXTDP_Responses(xtdp_str, pdu.xtdp__Message().xtdp__Responses());
                  break;
          case XTDP__PDU__Defs::XTDP__Message_xtdp__Message::ALT_xtdp__AddRequests:
                  xtdp_str = mputXTDP_Add(xtdp_str, pdu.xtdp__Message().xtdp__AddRequests());
                  break;
          case XTDP__PDU__Defs::XTDP__Message_xtdp__Message::ALT_xtdp__RemoveRequests:
                  xtdp_str = mputXTDP_Remove(xtdp_str, pdu.xtdp__Message().xtdp__RemoveRequests());
                  break;
          case XTDP__PDU__Defs::XTDP__Message_xtdp__Message::ALT_xtdp__LayoutRequest:
                    xtdp_str = mputXTDP_LayoutRequest(xtdp_str, pdu.xtdp__Message().xtdp__LayoutRequest());
                  break;
          case XTDP__PDU__Defs::XTDP__Message_xtdp__Message::ALT_xtdp__AuthChallenge:
                    xtdp_str = mputXTDP_AuthChallenge(xtdp_str, pdu.xtdp__Message().xtdp__AuthChallenge());
                  break;
          case XTDP__PDU__Defs::XTDP__Message_xtdp__Message::ALT_xtdp__LayoutResponse:
                    TTCN_error("LayoutResponse can not be sent to the Runtime GUI, only received.");
                  break;
          case XTDP__PDU__Defs::XTDP__Message_xtdp__Message::ALT_xtdp__AuthResponse:
                    TTCN_error("AuthResponse can not be sent to the Runtime GUI, only received.");
                  break;
          }

          xtdp_str = mputstr(xtdp_str, "</XTDP-Message>");
          CHARSTRING ret_val(mstrlen(xtdp_str),(const char*)xtdp_str);
          Free(xtdp_str);
	if (TTCN_Logger::log_this_event(TTCN_DEBUG)) {
        		TTCN_Logger::begin_event(TTCN_DEBUG);
        		TTCN_Logger::log_event("enc__XTDP__Message() return value: ");
        		ret_val.log();
  		TTCN_Logger::end_event();
  	}

	return ret_val;
  }

  CHARSTRING enct__XTDP__Message(const XTDP__PDU__Defs::XTDP__Message_template& pdu)
  {
    return enc__XTDP__Message(pdu.valueof());
  }


  XTDP__PDU__Defs::XTDP__Message dec__XTDP__Message(const CHARSTRING& stream)
  {
          if (TTCN_Logger::log_this_event(TTCN_DEBUG)) {
                  TTCN_Logger::begin_event(TTCN_DEBUG);
                  TTCN_Logger::log_event("dec__XTDP__Message() called with argument: ");
                  stream.log();
                  TTCN_Logger::end_event();
          }

          XTDP__PDU__Defs::XTDP__Message ret_val = parse_xtdp((const char*)stream);

          if (TTCN_Logger::log_this_event(TTCN_DEBUG)) {
                  	TTCN_Logger::begin_event(TTCN_DEBUG);
                  	TTCN_Logger::log_event("dec__XTDP__Message() return value: ");
                  	ret_val.log();
		TTCN_Logger::end_event();
	}

	return ret_val;
  }

  XTDP__PDU__Defs::XTDP__XML__Tag dec__XUL(const CHARSTRING& xul)
  {
      if (TTCN_Logger::log_this_event(TTCN_DEBUG)) {
        TTCN_Logger::begin_event(TTCN_DEBUG);
        TTCN_Logger::log_event("dec__XUL() called with argument: ");
        xul.log();
        TTCN_Logger::end_event();
      }

      XTDP__PDU__Defs::XTDP__XML__Tag ret_val = parse_xul((const char*)xul);

      if (TTCN_Logger::log_this_event(TTCN_DEBUG)) {
    		TTCN_Logger::begin_event(TTCN_DEBUG);
    		TTCN_Logger::log_event("dec__XUL() return value: ");
    		ret_val.log();
    		TTCN_Logger::end_event();
    	}

  	return ret_val;
  }
}
