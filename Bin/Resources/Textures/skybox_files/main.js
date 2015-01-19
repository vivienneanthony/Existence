//
// Copyright (c) 2008-2014 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

// Avoid `console` errors in browsers that lack a console.
(function() {
    var method;
    var noop = function () {};
    var methods = [
        'assert', 'clear', 'count', 'debug', 'dir', 'dirxml', 'error',
        'exception', 'group', 'groupCollapsed', 'groupEnd', 'info', 'log',
        'markTimeline', 'profile', 'profileEnd', 'table', 'time', 'timeEnd',
        'timeline', 'timelineEnd', 'timeStamp', 'trace', 'warn'
    ];
    var length = methods.length;
    var console = (window.console = window.console || {});

    while (length--) {
        method = methods[length];

        // Only stub undefined methods.
        if (!console[method]) {
            console[method] = noop;
        }
    }
}());

// Place any jQuery/helper plugins in here.
$(document).ready(function() {
  // Bust Travis CI build status image cache
  $('.build-status img').each(function() {
    var $this = $(this);
    $this.attr('src', $this.attr('src') + '&v=' + Math.random());
  });

  // This is a documentation page when it contains a document switcher
  var $documentSwitcher = $('#document-switcher');
  if ($documentSwitcher.length) {
    // Verify the links in the document switcher
    $('+ .dropdown-menu [href]', $documentSwitcher).each(function(i, elem) {
      // i == 0 is /HEAD/
      // Page is new when only exists in /HEAD/
      // Page is deprecated when not exists in /HEAD/ anymore
      $.ajax({ url: elem, type: 'HEAD', async: false, statusCode: {
        200: function() {
          $documentSwitcher.data('new', i == 0);
        },
        404: function() {
          // In any case, disable the list item when the link it contained does not exist in server
          $(elem).removeAttr('href').parent().addClass("disabled");
          if (i == 0) $documentSwitcher.addClass('deprecated-page');
        }
      }});
    });
    if ($documentSwitcher.data('new')) $documentSwitcher.addClass('new-page');

    // Inject font-awesome icons to top navigation, hide the navigation text on really really small viewport
    $('.tablist [href]').each(function(i, elem) {
      var icon;
      if (/index\.html$/.test(elem)) icon = '<i class="fa fa-home fa-lg"></i>&nbsp;';
      else if (/pages\.html$/.test(elem)) icon = '<i class="fa fa-file"></i>&nbsp;';
      else if (/modules\.html$/.test(elem)) icon = '<i class="fa fa-puzzle-piece"></i>&nbsp;';
      else if (/namespaces\.html$/.test(elem)) icon = '<i class="fa fa-tags"></i>&nbsp;';
      else if (/namespacemembers\.html$/.test(elem)) icon = '<i class="fa fa-tag"></i>&nbsp;';
      else if (/annotated\.html$/.test(elem)) icon = '<i class="fa fa-cube"></i>&nbsp;';
      else if (/classes\.html$/.test(elem)) icon = '<i class="fa fa-th"></i>&nbsp;';
      else if (/inherits\.html$/.test(elem)) icon = '<i class="fa fa-sitemap"></i>&nbsp;';
      else if (/functions\.html$/.test(elem)) icon = '<i class="fa fa-th-list"></i>&nbsp;';
      else if (/files\.html$/.test(elem)) icon = '<i class="fa fa-code"></i>&nbsp;';
      else if (/globals\.html$/.test(elem)) icon = '<i class="fa fa-globe"></i>&nbsp;';
      else if (/_func\.html$/.test(elem)) icon = '<i class="fa fa-bars"></i>&nbsp;';
      else if (/_vars\.html$/.test(elem)) icon = '<i class="fa fa-dot-circle-o"></i>&nbsp;';
      else if (/_type\.html$/.test(elem)) icon = '<i class="fa fa-circle-o"></i>&nbsp;';
      else if (/_enum\.html$/.test(elem)) icon = '<i class="fa fa-plus-square-o"></i>&nbsp;';
      else if (/_eval\.html$/.test(elem)) icon = '<i class="fa fa-square-o"></i>&nbsp;';
      else if (/_prop\.html$/.test(elem)) icon = '<i class="fa fa-cog"></i>&nbsp;';
      else if (/_rela\.html$/.test(elem)) icon = '<i class="fa fa-link"></i>&nbsp;';
      if (icon) $(elem).prepend(icon).find('span').addClass('hidden-xxs');
    });

    // Inject responsive behaviour to tables and embedded SVGs
    $('.contents table').wrap('<div class="table-responsive"></div>');
    // Use large version for graphical class hierarchy list and class member list
    if (/(inherits|-members)\.html$/.test(window.location.href)) $('.table-responsive').removeClass('table-responsive').addClass('table-responsive-lg');
    $('.zoom').addClass('embed-responsive embed-responsive-16by9');

    // Inject dropdown class to summary links
    $('.summary')
      .addClass('dropdown pull-right')
      .html(function() {
        return $(this).html().replace(/\|/g, '');
      })
      .prepend('<button class="btn btn-default btn-xs dropdown-toggle" type="button" id="class-summary" data-toggle="dropdown">Summary <span class="caret"></span></button>')
      .find('a')
        .wrapAll('<ul class="dropdown-menu" role="menu" aria-labelledby="class-summary"></ul>')
        .wrap('<li role="presentation"></li>')
        .attr('role', 'menuitem').attr('tabindex', '-1');

    // Bug fix for WebKit - additional 56px due to WebKit bug not taking the img width into consideration and causing overall td width not wide enough for the text
    if (/(chrom(e|ium)|applewebkit)/.test(navigator.userAgent.toLowerCase())) $('.directory td.entry').css('padding-right', '62px'); // 6 + 56
  }
  else {
    // Show/hide binary packages based on package-filter's criteria
    $('.package-filter .criteria').change(function() {
      // There could be more than one package-filter in a page, so find the actual one containing the checkbox that received the change event
      var $packageFilter = $(this).parents('.package-filter');
      $packageFilter.data('criterias')[this.name] = this.checked; // Update the filter criteria
      // Then find the package grid which should be located next to the changed package-filter
      var data = $packageFilter.data('criterias');
      $packageFilter.next()
        .find('.binary-package')
          // Perform the actual filtering
          .find('.64-bit.STATIC').toggleClass('hidden', !(data['64-bit'] && data.STATIC)).end()
          .find('.64-bit.SHARED').toggleClass('hidden', !(data['64-bit'] && data.SHARED)).end()
          .find('.32-bit.STATIC').toggleClass('hidden', !(data['32-bit'] && data.STATIC)).end()
          .find('.32-bit.SHARED').toggleClass('hidden', !(data['32-bit'] && data.SHARED)).end()
          .each(function() {
            // Hide list header when the list itself is hidden
            var $this = $(this);
            $this.prev().toggleClass('hidden', !$this.find('li:not(.hidden)').length);
          }).end()
        .masonry(); // Relayout the package grid to minimize unused space
    }).each(function() {
      // Ensure package-filter's criterias agree with their default values on page reload
      this.checked = this.defaultChecked;
    });

    // Give some time for Disqus async content to populate then scroll to the anchor one more time
    $(window).on("load", function() {
      if (/disqus_thread$/.test(window.location.hash)) {
        setTimeout(function() {
          $('html, body').scrollTop($(window.location.hash).position().top);
        }, 800);
      }
    });
  }
});
