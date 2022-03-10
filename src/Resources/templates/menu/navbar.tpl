<div class="main-header navbar navbar-expand {$main_header_classes}">

    <!-- Left navbar links -->
    <ul class="navbar-nav">
        <!-- Minimize Sidebar Button -->
        <li class="nav-item">
            <a class="nav-link" data-widget="pushmenu" href="#" role="button"><i class="fas fa-bars"></i></a>
        </li>

        {$menu['left']}

    </ul>

    <!-- Right navbar links -->
    <ul class="navbar-nav ml-auto">
        {$menu['right']}

        <li class="nav-item d-none">
            <a class="nav-link" data-widget="fullscreen" href="#" role="button">
                <i class="fas fa-expand-arrows-alt"></i>
            </a>
        </li>

    </ul>

    <script type="text/javascript">
        {literal}
        jQuery(function () {
            jQuery('.nav-item .nav-link[data-toggle="tooltip"]').tooltip()
        });
        {/literal}
    </script>

</div>
