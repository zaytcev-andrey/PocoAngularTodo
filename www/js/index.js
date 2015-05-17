// Button
$('#fab').on("click", function() {
  $("#card1").append("<div class='link'><div class='link-title'>Last link</div></div>");
  $("link:last").addClass('link');
});

//Parallax
$(window).scroll(function(e) {
  parallax();
});

function parallax() {
  var scrolled = $(window).scrollTop();
  $('.background').css('top', -(scrolled * 0.5) + 'px');
}
$(window).scroll(function() {
  if ($(window).scrollTop() > 20) {
    if ($(window).width() > 600) {
      $('#fab-wrap').addClass('hidden', 100, "linear");
    }
  } else {
    $('#fab-wrap').removeClass('hidden');
  }
});
